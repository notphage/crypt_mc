#pragma once

class c_client_handler
{
	int m_client_socket;
	c_connection m_connection;
public:
	c_client_handler(int client_socket)
		: m_client_socket(client_socket), m_connection(client_socket)
	{ }

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
