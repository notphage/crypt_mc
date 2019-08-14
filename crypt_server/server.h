#pragma once

#include "../packets.h"
#include "game_list.h"

#include <string_view>
#include <arpa/inet.h> //inet_addr

enum client_handler_init_t
{
	CHI_SUCCESS,
	CHI_FAILED_TLS,
	CHI_FAILED_TLS_FD,
	CHI_FAILED_TLS_ACCEPT
};

enum client_handler_stage_t
{
	CHS_VERSION,
	CHS_LOGIN,
	CHS_SELECTION,
	CHS_WATCHDOG
};

enum client_handler_packet_status_t
{
	CHPS_VALID,
	CHPS_INVALID,
	CHPS_NO_RECV
};

class c_client_handler : std::enable_shared_from_this<c_client_handler>
{
	SSL* m_ssl;
	std::string m_client_ip;
	int m_client_socket;
	sockaddr_in m_sock_addr;
	std::vector<c_game_entry> m_games;
	std::thread m_thread;
	std::atomic_bool m_disconnected = false;

	client_handler_stage_t m_stage = CHS_VERSION;

	c_connection m_connection;
	c_packet_handler m_packet_handler;

	template <typename T>
	client_handler_packet_status_t receive_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		if (m_connection.receive() == -1)
			return CHPS_NO_RECV;

		memcpy(&packet, m_connection.buffer_data(), sizeof packet);

		if (!m_packet_handler.validate_packet(packet))
			return CHPS_INVALID;
		
		m_packet_handler.xor_packet(packet);

		return CHPS_VALID;
	}

	template <typename T>
	client_handler_packet_status_t handle_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");
		
		switch (m_stage)
		{
			case CHS_VERSION:
			{
				if (auto ret = receive_packet(packet); ret != CHPS_VALID)
					return ret;

				if (packet.m_type != packet_type_t::PACKET_VERSION)
					return CHPS_INVALID;
				
				break;
			}
			
			case CHS_LOGIN:
			{
				if (auto ret = receive_packet(packet); ret != CHPS_VALID)
					return ret;

				if (packet.m_type != packet_type_t::PACKET_LOGIN)
					return CHPS_INVALID;

				break;
			}
			
			case CHS_SELECTION:
			{
				if (auto ret = receive_packet(packet); ret != CHPS_VALID)
					return ret;

				if (packet.m_type != packet_type_t::PACKET_CHEAT)
					return CHPS_INVALID;

				break;
			}
			
			case CHS_WATCHDOG:
			{
				if (auto ret = receive_packet(packet); ret != CHPS_VALID)
					return ret;

				if (packet.m_type != packet_type_t::PACKET_WATCHDOG)
					return CHPS_INVALID;

				break;
			}
			default:
			{
				return CHPS_NO_RECV;
			}
		}

		return CHPS_VALID;
	}
	
	void handle_client();
public:
	c_client_handler(const std::string& ip, const int client_socket, const sockaddr_in& sock_addr)
		: m_ssl(nullptr), m_client_ip(ip), m_client_socket(client_socket), m_sock_addr(sock_addr), m_connection(client_socket, ip)
	{ 
		m_games.emplace_back("Crypt Ghost", "lol.exe", "crypt_lol.dll", 1, game_packet_status_t::GAME_ONLINE);
	}

	client_handler_init_t initialize(SSL_CTX* ssl_context);
	void run();

	std::string_view get_ip_str()
	{
		return m_client_ip;
	}

	const sockaddr_in& get_sock_addr() const
	{
		return m_sock_addr;
	}

	bool is_disconnected() const
	{
		return m_disconnected;
	}
};

struct ip_infraction_t
{
	uint32_t m_ip;
	uint32_t m_infraction_count;
	uint64_t m_infraction_time;
};

class c_server
{
	SSL_CTX* m_ssl_ctx;
	
	std::vector<std::shared_ptr<c_client_handler>> m_clients;
	std::vector<ip_infraction_t> m_ip_infractions;
	std::mutex m_ip_infractions_mutex;

	int16_t m_port;
	int m_socket_desc = -1;

	void init_tls();
	void bind_socket();
public:
	explicit c_server(int16_t port);

	bool add_infraction(const std::shared_ptr<c_client_handler>& client);
	bool is_ip_banned(const sockaddr_in& client);
	void run();

};
