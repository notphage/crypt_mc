#include "context.h"

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write

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
		static char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client.sin_addr, ip_str, INET_ADDRSTRLEN);

		ctx.m_console.write(std::string("Accepting connection from ") + ip_str);

		ctx.m_clients++;

		m_thread_pool.enqueue([this](const std::string& ip, int client_socket)
			{
				c_client_handler client_handler(ip, client_socket);
				client_handler.run();
			}, ip_str, client_socket);
	}

	if (client_socket < 0)
		throw std::runtime_error("Accept Failed");

	return;
}

c_client_handler::~c_client_handler()
{
	shutdown(m_client_socket, SHUT_RDWR);

	ctx.m_clients--;
}

void c_client_handler::run()
{
	// Welcome String
	{
		std::string welcome("Welcome from the Crypt Server!");
		m_connection.set_buffer(welcome.data(), welcome.size());
		m_connection.send();
	}

	// Version Packets
	{
		auto version_packet = recieve_packet<c_version_packet>();

		if (ctx.m_required_version > version_packet.m_version)
		{
			printf("Client %s outdated version: %ull\n", m_client_ip.c_str(), ctx.m_required_version);

			version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, true);
			m_connection.set_buffer(&version_packet, sizeof version_packet);

			return;
		}

		printf("Client %s connected with a updated client.\n", m_client_ip.c_str());

		version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, false);
		m_connection.set_buffer(&version_packet, sizeof version_packet);
		m_connection.send();
	}

	// Login Packets
	{
		auto login_packet = recieve_packet<c_login_packet>();

		printf("Client %s logged into user %s.\n", m_client_ip.c_str(), login_packet.m_username);
	}

	// Game Packets
	{
		c_games_packet game_packet = m_packet_handler.create_games_packet("", game_packet_status_t::GAME_INVALID, (uint8_t)m_games.size());
		m_connection.set_buffer(&game_packet, sizeof game_packet);
		m_connection.send();

		for (auto&& game : m_games)
		{
			game_packet = m_packet_handler.create_games_packet(game.m_name, game.m_status, (uint8_t)m_games.size());
			m_connection.set_buffer(&game_packet, sizeof game_packet);
			m_connection.send();
		}
	}

	// Game Selection
	{
		m_connection.recieve();
		c_login_packet login_packet;
		memcpy(&login_packet, m_connection.buffer_data(), m_connection.buffer_size());

		m_packet_handler.xor_packet(login_packet);
	}
}
