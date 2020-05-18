#pragma once

#include "../packets.h"
#include "../shared_mem.h"
#include "game_list.h"
#include "../shared_queue.h"
#include "process.hpp"
#include "manualmapper.hpp"

class c_client
{
	std::atomic<connection_stage> m_conn_stage;
	std::thread m_socket_thread;
	std::atomic<shared_mem_stage> m_shared_mem_stage;
	std::thread m_shared_mem_thread;
	std::string m_username{};
	uint64_t m_password = 0;
	uint64_t m_hwid = 0;
	uint64_t m_cookie = 0;

	c_connection m_connection;
	c_packet_handler m_packet_handler;
	c_shared_mem_queue m_mem_queue;
	c_mem_handler m_mem_handler;
	c_protection m_protection;
	native::process* m_process;
	injection::manualmapper* m_mapper;

	template <typename T>
	bool receive_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		if (m_connection.receive() == SOCKET_ERROR)
			return false;

		RtlSecureZeroMemory(&packet, sizeof packet);
		memcpy(&packet, m_connection.buffer_data(), m_connection.buffer_size());

		m_packet_handler.xor_packet(packet);

		return true;
	}

	bool handle_connection();
	bool handle_login();

	void run_socket();
	void run_shared_mem();
public:
	c_client()
		: m_conn_stage(connection_stage::STAGE_WAITING), m_socket_thread(std::thread(&c_client::run_socket, this)),
		  m_shared_mem_stage(shared_mem_stage::STAGE_CREATE), m_shared_mem_thread(std::thread(&c_client::run_shared_mem, this)),
		  m_mem_queue(xors("Spotify"), 0x100000, c_shared_mem_queue::mode::server)
	{ }

	~c_client()
	{
		m_socket_thread.join();
		m_shared_mem_thread.join();

		delete m_process;
		delete m_mapper;
	}

	void set_conn_stage(connection_stage stage)
	{
		m_conn_stage = stage;
	}

	connection_stage get_conn_stage() const
	{
		return m_conn_stage;
	}

	void set_shared_mem_stage(shared_mem_stage stage)
	{
		m_shared_mem_stage = stage;
	}

	shared_mem_stage get_shared_mem_stage() const
	{
		return m_shared_mem_stage;
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