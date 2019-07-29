#pragma once

#include "../packets.h"
#include "game_list.h"

class c_client
{
	std::thread m_connection_thread;
	std::atomic<connection_stage> m_stage;
	std::string m_username;
	uint64_t m_password;
	uint64_t m_hwid;

	c_connection m_connection;
	c_packet_handler m_packet_handler;

	template <typename T>
	bool recieve_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		if (m_connection.recieve() == SOCKET_ERROR)
			return false;

		RtlSecureZeroMemory(&packet, sizeof packet);
		memcpy(&packet, m_connection.buffer_data(), m_connection.buffer_size());

		m_packet_handler.xor_packet(packet);

		return true;
	}

	void run();

public:
	c_client()
		: m_connection_thread(std::thread(&c_client::run, this)), m_stage(connection_stage::STAGE_WAITING)
	{ }

	void set_stage(connection_stage stage)
	{
		m_stage = stage;
	}

	connection_stage get_stage()
	{
		return m_stage;
	}

	__forceinline std::string game_status_to_string(game_packet_status_t status)
	{
		switch (status)
		{
		case game_packet_status_t::GAME_OFFLINE:
			return xors("OFFLINE");

		case game_packet_status_t::GAME_NOSUB:
			return xors("NO SUB");

		case game_packet_status_t::GAME_DOWN:
			return xors("DOWN");

		case game_packet_status_t::GAME_UPDATING:
			return xors("UPDATING");

		case game_packet_status_t::GAME_ONLINE:
			return xors("ONLINE");

		default:
			return xors("INVALID");
		}
	}

	__forceinline void set_login_info(const std::string& username, uint64_t password, uint64_t hwid)
	{
		m_username = username;
		m_password = password;
		m_hwid = hwid;
	}
};