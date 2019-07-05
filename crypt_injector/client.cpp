#include "context.h"

void c_client::run()
{
	m_connection.connect();

	// Welcome String
	{
		m_connection.recieve();
		std::string recv_str((const char*)m_connection.buffer_data(), m_connection.buffer_size());

		std::cout << std::endl << recv_str << std::endl;
	}

	// Version Packet
	{
		c_version_packet version_packet = m_packet_handler.create_version_packet(ctx.m_version, false);
		m_connection.set_buffer(&version_packet, sizeof version_packet);
		m_connection.send();

		version_packet = recieve_packet<c_version_packet>();

		if (version_packet.m_upgrade_required)
		{
			printf("Crypt Loader outdated. Required version: %llu Current Version: %llu\n", version_packet.m_version, ctx.m_version);
			return;
		}

		printf("Crypt Loader is an updated version.\n");
	}

	// Login Packet
	{
		c_login_packet login_packet = m_packet_handler.create_login_packet("Yur fucking gay", 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF);

		m_connection.set_buffer(&login_packet, sizeof login_packet);
		m_connection.send();
	}

	// Games Packets
	{
		auto games_packet = recieve_packet<c_games_packet>();

		for (auto i = 0; i < games_packet.m_num; i++)
		{
			games_packet = recieve_packet<c_games_packet>();

			c_game_entry game_entry;
			game_entry.m_name = std::string(games_packet.m_name);
			game_entry.m_status = games_packet.m_status;
			m_games.push_back(game_entry);
		}
	}

	size_t selected_game = handle_game_selection();
}

size_t c_client::handle_game_selection()
{
	if (m_games.empty())
	{
		printf("No games are currently available.\n");
		return 0;
	}

	size_t selected_game;

	for (auto i = 0; i < m_games.size(); i++)
		printf("> %i: %s (%s)\n", i, m_games.at(i).m_name.c_str(), game_status_to_string(m_games.at(i).m_status).c_str());

	printf("Please select your game: ");
	std::cin >> selected_game;

	return selected_game;
}
