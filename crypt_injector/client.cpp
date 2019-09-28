#include "context.h"
#include "process.hpp"
#include "manualmapper.hpp"
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
				if (!m_protection.init_safety_check())
				{
					ctx.m_loader_window.get_gui().insert_text(m_protection.m_err_str, color_t(255, 0, 0));
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));

					std::this_thread::sleep_for(std::chrono::seconds(5));

					ctx.m_panic = true;
					return;
				}
				
				ctx.m_loader_window.get_gui().insert_text(xors("Connecting..."), color_t(255, 255, 255));

				if (!m_connection.connect())
				{
					m_conn_stage = connection_stage::STAGE_INVALID;
					ctx.m_loader_window.get_gui().insert_text(xors("Couldn't establish a connection"), color_t(255, 0, 0));
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));

					std::this_thread::sleep_for(std::chrono::seconds(5));

					ctx.m_panic = true;
					return;
				}
				
				if (!m_connection.tls_connect())
				{
					m_conn_stage = connection_stage::STAGE_INVALID;
					ctx.m_loader_window.get_gui().insert_text(xors("Couldn't establish a secure connection"), color_t(255, 0, 0));
					ctx.m_loader_window.get_gui().insert_text(std::string(xors("Closing in 5 seconds...")), color_t(255, 255, 255));
				
					std::this_thread::sleep_for(std::chrono::seconds(5));
				
					ctx.m_panic = true;
					return;
				}

				ctx.m_loader_window.get_gui().insert_text(xors("Success"), color_t(255, 255, 255));

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
					c_login_packet login_packet = m_packet_handler.create_login_packet(m_username, m_password, m_hwid, login_packet_status_t::LOGIN_DUMMY);

					m_connection.set_buffer(&login_packet, sizeof login_packet);
					if (m_connection.send() == SOCKET_ERROR)
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					login_packet = {};
					if (!receive_packet<c_login_packet>(login_packet))
					{
						m_conn_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					switch (login_packet.m_status)
					{
						case login_packet_status_t::LOGIN_INVALID:
						{
							ctx.m_loader_window.get_gui().insert_text(std::string(xors("Incorrect username/password")), color_t(255, 0, 0));
							m_conn_stage = connection_stage::STAGE_CLOSE;
							break;
						}

						case login_packet_status_t::LOGIN_MISMATCH:
						{
							ctx.m_loader_window.get_gui().insert_text(std::string(xors("Connection error #2")), color_t(255, 0, 0));
							m_conn_stage = connection_stage::STAGE_CLOSE;
							break;
						}

						case login_packet_status_t::LOGIN_VALID:
						{
							ctx.m_loader_window.get_gui().insert_text(std::string(xors("Logged in successfully")), color_t(255, 255, 255));
							break;
						}

						case login_packet_status_t::LOGIN_DUMMY:
						default:
						{
							ctx.m_loader_window.get_gui().insert_text(std::string(xors("Connection error #1")), color_t(255, 0, 0));
							m_conn_stage = connection_stage::STAGE_CLOSE;
							break;
						}
					}

					if (login_packet.m_status != login_packet_status_t::LOGIN_VALID)
						break;
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
					strcpy(const_cast<char*>(ctx.m_window_class.c_str()), cheat_packet.m_window_class);
				}

				// Cheat Data
				{
					m_connection.receive();
					std::copy(m_connection.get_buffer().data(), m_connection.get_buffer().data() + m_connection.get_buffer().size(), std::back_inserter(ctx.m_buffer));
				}

				{
					m_process = new native::process(ctx.m_window_class, {}, PROCESS_ALL_ACCESS);
					m_mapper = new injection::manualmapper(*m_process);

					const auto address = m_mapper->inject(ctx.m_buffer, injection::executor::mode::CREATE_THREAD);

					if (address)
					{
						ctx.m_loader_window.get_gui().insert_text(std::string(xors("Injection successful")), color_t(255, 255, 255));
						m_conn_stage = connection_stage::STAGE_WATCHDOG;
					}
					else
					{
						ctx.m_loader_window.get_gui().insert_text(std::string(xors("Injection failed")), color_t(255, 255, 255));
						m_conn_stage = connection_stage::STAGE_CLOSE;
					}
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
					break;
				}

				static auto old = std::chrono::high_resolution_clock::now();
				auto now = std::chrono::high_resolution_clock::now();
				uint32_t delta;

				do
				{
					now = std::chrono::high_resolution_clock::now();
					delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
					std::this_thread::sleep_for(std::chrono::seconds(1));
				} while (delta < 3000);

				old = now;

				auto status = watchdog_packet_status_t::WATCHDOG_KEEPALIVE;

				c_watchdog_packet watchdog_packet{};

				if (!ctx.m_watchdog)
					status = watchdog_packet_status_t::WATCHDOG_CLOSE;

				// Run detections
				//if (!m_protection.safety_check())
				//{
				//	m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
				//	watchdog_packet = m_packet_handler.create_watchdog_packet(watchdog_packet_status_t::WATCHDOG_BAN, m_protection.m_err_str);
				//}
				//else
				{
					watchdog_packet = m_packet_handler.create_watchdog_packet(status);
				}
				
				m_connection.set_buffer(&watchdog_packet, sizeof watchdog_packet);
				if (m_connection.send() == SOCKET_ERROR || status != watchdog_packet_status_t::WATCHDOG_KEEPALIVE)
				{
					m_connection.disconnect();
					ctx.m_panic = true;
					ctx.m_watchdog = false;
					
					return;
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
	while (m_shared_mem_stage != shared_mem_stage::STAGE_INVALID)
	{
		if (!ctx.m_watchdog)
			continue;
		
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

				const auto base_mem_packet = reinterpret_cast<c_mem_packet*>(msg.m_data.data());
				switch (base_mem_packet->m_type)
				{
					case mem_type_t::MEM_HOOK:
					{
						m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
						
						break;
					}
					
					case mem_type_t::MEM_PING:
					{
						const auto ping_packet = reinterpret_cast<c_mem_ping_packet*>(msg.m_data.data());
						if (!m_mem_handler.validate_mem_packet(*ping_packet))
						{
							m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

							break;
						}
						m_mem_handler.xor_mem_packet(*ping_packet);

						if (ping_packet->m_hash == 0)
						{
							m_shared_mem_stage = shared_mem_stage::STAGE_CLOSE;

							break;
						}

						if (ping_packet->m_hash != m_cookie)
						{
							m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;

							break;
						}

						m_cookie = fnvr(ping_packet->m_hash);
						
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
				m_cookie = fnvr(fnvr(m_username.c_str()) ^ m_hwid);
				auto init_packet = m_mem_handler.create_init_mem_packet(m_username.c_str(), m_cookie);
				m_cookie = fnvr(m_cookie);

				const mem_message_t msg(reinterpret_cast<uint8_t*>(&init_packet), sizeof init_packet);
				
				if (!m_mem_queue.push_message(msg))
					m_shared_mem_stage = shared_mem_stage::STAGE_FORCECLOSE;
				else
					m_shared_mem_stage = shared_mem_stage::STAGE_WAITING;
				
				break;
			}

			case shared_mem_stage::STAGE_CLOSE:
			{
				std::this_thread::sleep_for(std::chrono::seconds(5));

				if (!m_mapper->uninject(injection::executor::mode::CREATE_THREAD))
				{
					system(xors("taskkill /F /T /IM javaw.exe"));
				}

				ctx.m_watchdog = false;

				return;
			}

			case shared_mem_stage::STAGE_FORCECLOSE:
			{
				auto proc = native::process(ctx.m_window_class, {}, PROCESS_ALL_ACCESS);
				auto mapper = injection::manualmapper(proc);

				if (!mapper.uninject(injection::executor::mode::CREATE_THREAD))
				{
					system(xors("taskkill /F /T /IM javaw.exe"));
				}

				ctx.m_watchdog = false;

				return;
			}
			
			default: ;
		}
	}
}
