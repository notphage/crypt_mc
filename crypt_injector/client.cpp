#include "context.h"
#include <chrono>

void c_client::run()
{
	while (m_stage != connection_stage::STAGE_INVALID)
	{
		switch (m_stage)
		{
			case connection_stage::STAGE_WAITING:
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));

				break;
			}

			case connection_stage::STAGE_LOGIN:
			{
				ctx.m_loader_window.get_gui().insert_text("Connecting...", color_t(255, 255, 255));

				if (!m_connection.connect())
				{
					m_stage = connection_stage::STAGE_INVALID;
					ctx.m_loader_window.get_gui().insert_text("Couldn't establish a connection.", color_t(255, 0, 0));
					ctx.m_loader_window.get_gui().insert_text(std::string("Closing in 5 seconds..."), color_t(255, 255, 255));

					std::this_thread::sleep_for(std::chrono::seconds(5));

					ctx.m_panic = true;
					return;
				}

				ctx.m_loader_window.get_gui().insert_text("Success.", color_t(255, 255, 255));

				// Version Packet
				{
					c_version_packet version_packet = m_packet_handler.create_version_packet(ctx.m_version, false);
					m_connection.set_buffer(&version_packet, sizeof version_packet);
					if (m_connection.send() == SOCKET_ERROR)
					{
						m_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					if (!recieve_packet<c_version_packet>(version_packet))
					{
						m_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					if (version_packet.m_upgrade_required)
					{
						ctx.m_loader_window.get_gui().insert_text(std::string("OUTDATED. Required: ") + std::to_string(version_packet.m_version) + std::string(" Current: ") + std::to_string(ctx.m_version), color_t(255, 255, 255));
						m_stage = connection_stage::STAGE_INVALID;

						ctx.m_loader_window.get_gui().insert_text(std::string("Closing in 5 seconds..."), color_t(255, 255, 255));

						std::this_thread::sleep_for(std::chrono::seconds(5));

						ctx.m_panic = true;
						return;
					}

					ctx.m_loader_window.get_gui().insert_text(std::string("UPDATED. Version: ") + std::to_string(ctx.m_version), color_t(255, 255, 255));
				}

				// Login Packet
				{
					c_login_packet login_packet = m_packet_handler.create_login_packet("Yur fucking gay", 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);

					m_connection.set_buffer(&login_packet, sizeof login_packet);
					if (m_connection.send() == SOCKET_ERROR)
					{
						m_stage = connection_stage::STAGE_CLOSE;
						break;
					}
				}

				// Games Packets
				{
					c_games_packet games_packet;
					if (!recieve_packet<c_games_packet>(games_packet))
					{
						m_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					std::lock_guard<std::mutex> lock(ctx.m_game_list_mutex);

					for (auto i = 0; i < games_packet.m_num; i++)
					{
						if (!recieve_packet<c_games_packet>(games_packet))
						{
							m_stage = connection_stage::STAGE_CLOSE;
							break;
						}

						ctx.m_game_list.emplace_back(std::string(games_packet.m_name), games_packet.m_version, games_packet.m_status);
					}
				}

				m_stage = connection_stage::STAGE_WAITING;
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
						m_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					if (!recieve_packet<c_cheat_packet>(cheat_packet))
					{
						m_stage = connection_stage::STAGE_CLOSE;
						break;
					}

					ctx.m_loader_window.get_gui().insert_text(std::string("Class: ") + cheat_packet.m_window_class, color_t(255, 255, 255));
					ctx.m_window_class = cheat_packet.m_window_class;
				}

				// Cheat Data
				{
					m_connection.recieve();
					std::copy(m_connection.get_buffer().data(), m_connection.get_buffer().data() + m_connection.get_buffer().size(), std::back_inserter(ctx.m_buffer));
				}

				ctx.m_injector.inject_from_memory(ctx.m_buffer.data());
				ctx.m_buffer.clear();

				m_stage = connection_stage::STAGE_CLOSE;
				break;
			}

			case connection_stage::STAGE_CLOSE:
			{
				m_connection.disconnect();
				ctx.m_loader_window.get_gui().insert_text(std::string("Disconnected from server"), color_t(255, 255, 255));
				ctx.m_loader_window.get_gui().insert_text(std::string("Closing in 5 seconds..."), color_t(255, 255, 255));

				std::this_thread::sleep_for(std::chrono::seconds(5));
				ctx.m_panic = true;
				return;
			}
		}
	}
}
