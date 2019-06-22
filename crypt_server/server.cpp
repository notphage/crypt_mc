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
	int client_socket = -1, c = sizeof(struct sockaddr_in);
	struct sockaddr_in client;

	while ((client_socket = accept(m_socket_desc, (struct sockaddr*)&client, (socklen_t*)&c)))
	{
		static char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client.sin_addr, ip_str, INET_ADDRSTRLEN);

		ctx.m_console.write(std::string("Accepting connection from ") + ip_str);

		ctx.m_clients++;

		m_thread_pool.enqueue([this](int client_socket)
			{
				c_client_handler client_handler(client_socket);
				client_handler.run();
			}, client_socket);
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
	std::string welcome("Welcome from the Crypt Server!");
	m_connection.set_buffer(welcome.data(), welcome.size());
	m_connection.send();
}
