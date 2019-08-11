#pragma once

#include "../packets.h"
#include "game_list.h"

enum client_handler_init_t
{
	CHI_SUCCESS,
	CHI_FAILED_TLS,
	CHI_FAILED_TLS_FD,
	CHI_FAILED_TLS_ACCEPT
};

class c_client_handler
{
	SSL* m_ssl;
	std::string m_client_ip;
	int m_client_socket;
	std::vector<c_game_entry> m_games;
	std::thread m_thread;

	c_connection m_connection;
	c_packet_handler m_packet_handler;

	template <typename T>
	bool receive_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		if (m_connection.receive() == -1)
			return false;

		memcpy(&packet, m_connection.buffer_data(), sizeof packet);

		m_packet_handler.xor_packet(packet);

		return true;
	}

	void handle_client();
public:
	c_client_handler(const std::string& ip, int client_socket)
		: m_client_ip(ip), m_client_socket(client_socket), m_connection(client_socket, ip)
	{ 
		m_games.emplace_back("Crypt Ghost", "lol.exe", "crypt_lol.dll", 1, game_packet_status_t::GAME_ONLINE);
	}

	~c_client_handler();

	client_handler_init_t initialize(SSL_CTX* ssl_context);
	void run();
};

class c_server
{
	SSL_CTX* m_ssl_ctx;
	
	std::vector<std::shared_ptr<c_client_handler>> m_clients;

	int16_t m_port;
	int m_socket_desc = -1;

	void init_tls();
	void bind_socket();
public:
	c_server(int16_t port);

	void run();
};
