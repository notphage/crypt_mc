#include "context.h"
#include "cheat.h"

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <cmath>

c_server::c_server(int16_t port)
	: m_thread_pool(std::thread::hardware_concurrency()), m_port(port)
{
	printf("Crypt Server starting up... Initializing with %i threads running on port %i.\n", std::thread::hardware_concurrency(), m_port);

	struct sockaddr_in server;

	//Create socket
	m_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_desc == -1)
		throw std::runtime_error("Could not create socket");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
	server.sin_port = htons(8123);

	//Bind
	if (bind(m_socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0)
		throw std::runtime_error("Could not bind socket");

	//Listen
	listen(m_socket_desc, 3);

	//Accept and incoming connection
	puts("Crypt Server successfully binded. Waiting for incoming connections...");
}

void c_server::run()
{
	srand(time(0));

	int client_socket = -1, c = sizeof(struct sockaddr_in);
	struct sockaddr_in client;

	while ((client_socket = accept(m_socket_desc, (struct sockaddr*)&client, (socklen_t*)&c)))
	{
		struct timeval tv;
		tv.tv_sec = 60;
		tv.tv_usec = 0;
		setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
		setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

		static char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client.sin_addr, ip_str, INET_ADDRSTRLEN);

		ctx.m_clients++;

		m_thread_pool.enqueue([this](const std::string& ip, int client_socket)
			{
				c_client_handler client_handler(ip, client_socket);
				client_handler.run();
			}, ip_str, client_socket);
	}

	if (client_socket < 0)
		throw std::runtime_error("Accept Failed");

	close(m_socket_desc);

	return;
}

c_client_handler::~c_client_handler()
{
	m_connection.disconnect();

	ctx.m_clients--;
}

void c_client_handler::run()
{
	// Version Packets
	{
		c_version_packet version_packet;
		if (!recieve_packet<c_version_packet>(version_packet))
			return;

		if (ctx.m_required_version > version_packet.m_version)
		{
			ctx.m_console.write(std::string("Client ") + m_client_ip + std::string(" connected with an outdated version: ") + std::to_string(version_packet.m_version));

			version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, true);
			m_connection.set_buffer(&version_packet, sizeof version_packet);
			m_connection.send();

			return;
		}

		ctx.m_console.write(std::string("Client ") + m_client_ip + std::string(" connected with an updated client"));

		version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, false);
		m_connection.set_buffer(&version_packet, sizeof version_packet);
		m_connection.send();
	}

	// Login Packets
	{
		c_login_packet login_packet;
		if (!recieve_packet<c_login_packet>(login_packet))
			return;

		ctx.m_console.write(std::string("Client ") + m_client_ip + std::string(" logged into user ") + login_packet.m_username);
	}

	// Game Packets
	{
		c_games_packet game_packet = m_packet_handler.create_games_packet("", 0, 0, (uint8_t)m_games.size(), game_packet_status_t::GAME_INVALID);
		m_connection.set_buffer(&game_packet, sizeof game_packet);
		m_connection.send();

		for (auto&& game : m_games)
		{
			game_packet = m_packet_handler.create_games_packet(game.m_name, 1, 30, (uint8_t)m_games.size(), game.m_status);
			m_connection.set_buffer(&game_packet, sizeof game_packet);
			m_connection.send();
		}
	}

	size_t num_packets = (size_t)std::ceil((float)crypt_mc_dll_size / (float)max_data_len);

	// Game Selection
	c_cheat_packet cheat_packet;
	{
		if (!recieve_packet<c_cheat_packet>(cheat_packet))
			return;

		cheat_packet = m_packet_handler.create_cheat_packet("LWJGL", cheat_packet.m_id);
		m_connection.set_buffer(&cheat_packet, sizeof cheat_packet);
		m_connection.send();
	}

	// Send Cheat
	{
		m_connection.set_buffer(crypt_mc_dll, crypt_mc_dll_size);
		m_connection.send();

		//size_t bytes_read = 0;
		//for (auto i = 0; i < num_packets; i++)
		//{
		//	c_data_packet* data_packet = m_packet_handler.create_data_packet(crypt_mc_dll_size, crypt_mc_dll, bytes_read);
		//	m_connection.set_buffer(data_packet, sizeof *data_packet);
		//	m_connection.send();
		//
		//	if (data_packet)
		//	{
		//		delete data_packet;
		//		data_packet = nullptr;
		//	}
		//}
	}
}
