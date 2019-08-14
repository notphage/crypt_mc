#include "context.h"
#include <chrono>

void c_client::run_socket()
{
	while (m_conn_stage != connection_stage::STAGE_INVALID)
	{
		switch (m_conn_stage)
		{
			case connection_stage::STAGE_WAITING:
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));

				break;
			}

			case connection_stage::STAGE_LOGIN:
			{
				ctx.m_loader_window.get_gui().insert_text(xors("Connecting..."), color_t(255, 255, 255));

				if (!m_connection.connect())
				{
					m_conn_stage = connection_stage::STAGE_INVALID;
					ctx.m_loader_window.get_gui().insert_text(xors("Couldn't establish a connection."), color_t(255, 0, 0));
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));

					std::this_thread::sleep_for(std::chrono::seconds(5));

					ctx.m_panic = true;
					return;
				}
				
				if (!m_connection.tls_connect())
				{
					m_conn_stage = connection_stage::STAGE_INVALID;
					ctx.m_loader_window.get_gui().insert_text(xors("Couldn't establish a secure connection."), color_t(255, 0, 0));
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));
				
					std::this_thread::sleep_for(std::chrono::seconds(5));
				
					ctx.m_panic = true;
					return;
				}

				ctx.m_loader_window.get_gui().insert_text(xors("Success."), color_t(255, 255, 255));

				// Version Packet
				{
					c_version_packet version_packet = m_packet_handler.create_version_packet(ctx.m_version, false);
					m_connection.set_buffer(&version_packet, sizeof version_packet);
					if (m_connection.send() == SOCKET_ERROR)
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					if (!receive_packet<c_version_packet>(version_packet))
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					if (version_packet.m_upgrade_required)
					{
						ctx.m_loader_window.get_gui().insert_text(std::string(xors("Please update the loader")), color_t(255, 255, 255));
						
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}
				}

				// Login Packet
				{
					c_login_packet login_packet = m_packet_handler.create_login_packet(m_username, m_password, m_hwid);

					m_connection.set_buffer(&login_packet, sizeof login_packet);
					if (m_connection.send() == SOCKET_ERROR)
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}
				}

				// Games Packets
				{
					c_games_packet games_packet;
					if (!receive_packet<c_games_packet>(games_packet))
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					std::lock_guard<std::mutex> lock(ctx.m_game_list_mutex);

					for (auto i = 0; i < games_packet.m_num; i++)
					{
						if (!receive_packet<c_games_packet>(games_packet))
						{
							m_conn_stage = connection_stage::STAGE_CLOSE;
							break;
						}

						ctx.m_game_list.emplace_back(std::string(games_packet.m_name), games_packet.m_version, games_packet.m_status);
					}
				}

				m_conn_stage = connection_stage::STAGE_WAITING;
				break;
			}

			case connection_stage::STAGE_SELECT:
			{
				c_cheat_packet cheat_packet = m_packet_handler.create_cheat_packet("", ctx.m_selected_cheat);

				// Cheat Packets
				{
					m_connection.set_buffer(&cheat_packet, sizeof cheat_packet);
					if (m_connection.send() == SOCKET_ERROR)
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					if (!receive_packet<c_cheat_packet>(cheat_packet))
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					ctx.m_window_class.resize(64);
					memcpy(ctx.m_window_class.data(), cheat_packet.m_window_class, sizeof cheat_packet.m_window_class);
					ctx.m_window_class.resize(6);
				}

				// Cheat Data
				{
					m_connection.receive();
					std::copy(m_connection.get_buffer().data(), m_connection.get_buffer().data() + m_connection.get_buffer().size(), std::back_inserter(ctx.m_buffer));
				}

				if (ctx.m_injector.inject_from_memory(ctx.m_buffer.data()))
				{
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Injection successful")), color_t(255, 255, 255));
					m_conn_stage = connection_stage::STAGE_WATCHDOG;
				}
				else
				{
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Injection failed")), color_t(255, 255, 255));
					m_conn_stage = connection_stage::STAGE_CLOSE;
				}

				ctx.m_buffer.clear();
				ctx.m_buffer.resize(0);
				
				break;
			}

			case connection_stage::STAGE_WATCHDOG:
			{
				if (!ctx.m_watchdog)
				{
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Disconnected from server")), color_t(255, 255, 255));
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));

					std::this_thread::sleep_for(std::chrono::seconds(5));
					ctx.m_watchdog = true;
				}

				break;
			}

			case connection_stage::STAGE_CLOSE:
			{
				m_connection.disconnect();
				ctx.m_loader_window.get_gui().insert_text(std::string(xors("Disconnected from server")), color_t(255, 255, 255));
				ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));

				std::this_thread::sleep_for(std::chrono::seconds(5));
				ctx.m_panic = true;
				return;
			}
		}
	}
}

void c_client::run_shared_mem()
{
	while (m_shared_mem_stage != shared_mem_stage::STAGE_INVALID && ctx.m_watchdog)
	{
		switch (m_shared_mem_stage)
		{
			case shared_mem_stage::STAGE_WAITING:
			{
				if (!m_mem_queue.wait_for_message())
				{
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

					break;
				}

				mem_message_t msg;
				if (!m_mem_queue.pop_message(msg))
				{
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

					break;
				}

				auto base_mem_packet = reinterpret_cast<c_mem_packet*>(msg.m_data.data());
				switch (base_mem_packet->m_type)
				{
					case mem_type_t::MEM_HOOK:
					{
						m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
						
						break;
					}
					
					case mem_type_t::MEM_PING:
					{
						if (!m_mem_handler.validate_mem_packet(base_mem_packet))
						{
							m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

							break;
						}
						
						break;
					}
					
					default:
					{
						m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

						break;
					}
				}

				break;
			}
			
			case shared_mem_stage::STAGE_CREATE:
			{
				auto hook_packet = m_mem_handler.create_hook_mem_packet(0);

				const mem_message_t msg(reinterpret_cast<uint8_t*>(&hook_packet), sizeof hook_packet);
				
				if (!m_mem_queue.push_message(msg))
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
				else
					m_shared_mem_stage = shared_mem_stage::STAGE_WAITING;
				
				break;
			}
			
			default: ;
		}
	}
}
