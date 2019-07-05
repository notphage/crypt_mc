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
	T recieve_packet()
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		m_connection.recieve();
		T packet;
		memcpy(&packet, m_connection.buffer_data(), m_connection.buffer_size());

		m_packet_handler.xor_packet(packet);

		return packet;
	}

public:
	c_client_handler(const std::string& ip, int client_socket)
		: m_client_ip(ip), m_client_socket(client_socket), m_connection(client_socket)
	{ 
		m_games.emplace_back("CS:GO",                                "csgo.exe", "crypt_csgo.dll", game_packet_status_t::GAME_OFFLINE);
		m_games.emplace_back("CS:GO",                                "csgo.exe", "crypt_csgo.dll", game_packet_status_t::GAME_NOSUB);
		m_games.emplace_back("CS:GO",                                "csgo.exe", "crypt_csgo.dll", game_packet_status_t::GAME_DOWN);
		m_games.emplace_back("CS:GO",                                "csgo.exe", "crypt_csgo.dll", game_packet_status_t::GAME_UPDATING);
		m_games.emplace_back("CS:GO",                                "csgo.exe", "crypt_csgo.dll", game_packet_status_t::GAME_ONLINE);
		m_games.emplace_back("Meinkraft",                            "java.exe", "crypt_mc.dll",   game_packet_status_t::GAME_ONLINE);
		m_games.emplace_back("Meinkraft 2: Electric Boogaloo",       "java.exe", "crypt_mc.dll",   game_packet_status_t::GAME_ONLINE);
		m_games.emplace_back("Meinkraft 3: Adventures with Hypixel", "java.exe", "crypt_mc.dll",   game_packet_status_t::GAME_ONLINE);
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
