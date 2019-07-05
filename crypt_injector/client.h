#pragma once

#include "../packets.h"
#include "game_list.h"

class c_client
{
	std::vector<c_game_entry> m_games;

	c_connection m_connection;
	c_packet_handler m_packet_handler;

	__forceinline std::string game_status_to_string(game_packet_status_t status)
	{
		switch (status)
		{
		case game_packet_status_t::GAME_OFFLINE:
			return xors("OFFLINE");

		case game_packet_status_t::GAME_NOSUB:
			return xors("NO SUBSCRIPTION");

		case game_packet_status_t::GAME_DOWN:
			return xors("DOWN");

		case game_packet_status_t::GAME_UPDATING:
			return xors("OFFLINE");

		case game_packet_status_t::GAME_ONLINE:
			return xors("OFFLINE");

		default:
			return xors("INVALID");
		}
	}

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
	void run();
	size_t handle_game_selection();
};