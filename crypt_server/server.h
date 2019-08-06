#pragma once

#include "../packets.h"
#include "game_list.h"

class c_client_handler
{
	std::string m_client_ip;
	int m_client_socket;
	std::vector<c_game_entry> m_games;

	c_connection m_connection;
	c_packet_handler m_packet_handler;

	template <typename T>
	bool recieve_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		if (m_connection.recieve() == -1)
			return false;

		memcpy(&packet, m_connection.buffer_data(), sizeof packet);

		m_packet_handler.xor_packet(packet);

		return true;
	}

public:
	c_client_handler(const std::string& ip, int client_socket)
		: m_client_ip(ip), m_client_socket(client_socket), m_connection(client_socket, ip)
	{ 
		m_games.emplace_back("Crypt Ghost", "lol.exe", "crypt_lol.dll", 1, game_packet_status_t::GAME_ONLINE);
	}

	~c_client_handler();

	void run();
};

class c_server
{
	c_thread_pool m_thread_pool;

	int16_t m_port;
	int m_socket_desc = -1;
public:
	c_server(int16_t port);

	void run();
};
