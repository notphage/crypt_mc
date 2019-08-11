#include "context.h"
#include "cheat.h"
#include "cert.h"
#include "key.h"

#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <cmath>
#include <tuple>

c_server::c_server(int16_t port)
	: m_port(port)
{
	printf("> Server | Threads: %i Port: %i\n", std::thread::hardware_concurrency(), m_port);
}

void c_server::init_tls()
{
	// Internal SSL init functions
	SSL_library_init();
	SSL_load_error_strings();

	// Authentication
	m_ssl_ctx = SSL_CTX_new(TLS_server_method());
	if (!m_ssl_ctx)
		throw std::runtime_error("Couldn't create SSL/TLS context\n");

	if (SSL_CTX_set_min_proto_version(m_ssl_ctx, TLS1_2_VERSION) == 0)
		throw std::runtime_error("Couldn't set minimum version to TLS 1.2");

	BIO* cbio = BIO_new_mem_buf(crypt_cert, crypt_cert_size);
	X509* cert = PEM_read_bio_X509(cbio, nullptr, nullptr, nullptr);
	if (cert == nullptr)
		throw std::runtime_error("Couldn't load certificate\n");
	SSL_CTX_use_certificate(m_ssl_ctx, cert);

	BIO* kbio = BIO_new_mem_buf(crypt_pem, crypt_pem_size);
	RSA* rsa = PEM_read_bio_RSAPrivateKey(kbio, nullptr, nullptr, nullptr);
	if (rsa == nullptr)
		throw std::runtime_error("Couldn't load private key\n");
	SSL_CTX_use_RSAPrivateKey(m_ssl_ctx, rsa);

	const int key_check_code = SSL_CTX_check_private_key(m_ssl_ctx);
	if (key_check_code != 1)
		throw std::runtime_error("Couldn't verify private key\n");

	printf("> SLL | Access granted\n");
}

void c_server::bind_socket()
{
	struct sockaddr_in server;

	//Create socket
	m_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket_desc == -1)
		throw std::runtime_error("Could not create socket");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
	server.sin_port = htons(8123);

	//Bind
	if (bind(m_socket_desc, reinterpret_cast<struct sockaddr*>(&server), sizeof(server)) < 0)
		throw std::runtime_error("Could not bind socket");

	//Listen
	listen(m_socket_desc, 3);

	printf("> Server | Initialized sucessfully\n");
}

void c_server::run()
{
	srand(time(nullptr));
	init_tls();
	bind_socket();

	int client_socket = -1, c = sizeof(struct sockaddr_in);
	struct sockaddr_in client;

	while ((client_socket = accept(m_socket_desc, reinterpret_cast<struct sockaddr*>(&client), reinterpret_cast<socklen_t*>(&c))))
	{
		struct timeval tv;
		tv.tv_sec = 30;
		tv.tv_usec = 0;
		setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof tv);
		setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

		static char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client.sin_addr, ip_str, INET_ADDRSTRLEN);

		auto client_handler = std::make_shared<c_client_handler>(ip_str, client_socket);
		const auto ret = client_handler->initialize(m_ssl_ctx);

		if (ret == CHI_SUCCESS)
		{
			m_clients.push_back(std::move(client_handler));
			m_clients.back()->run();

			printf("> Server | Client %s accepted\n", ip_str);
		}
		else
		{
			printf("> Server | Client %s rejected for %i\n", ip_str, ret);
		}
	}

	if (client_socket < 0)
		throw std::runtime_error("Accept Failed");

	close(m_socket_desc);
}

c_client_handler::~c_client_handler()
{
	m_connection.disconnect();
}

client_handler_init_t c_client_handler::initialize(SSL_CTX* ssl_context)
{
	if ((m_ssl = SSL_new(ssl_context)) == nullptr)
		return CHI_FAILED_TLS;

	if (SSL_set_fd(m_ssl, m_client_socket) == 0)
		return CHI_FAILED_TLS_FD;

	if (SSL_accept(m_ssl) < 0)
		return CHI_FAILED_TLS_ACCEPT;

	m_connection.set_tls(m_ssl);

	return CHI_SUCCESS;
}

void c_client_handler::run()
{
	m_thread = std::thread(&c_client_handler::handle_client, this);
}

void c_client_handler::handle_client()
{
	// Version Packets
	{
		c_version_packet version_packet;
		if (!receive_packet<c_version_packet>(version_packet))
			return;

		if (ctx.m_required_version > version_packet.m_version)
		{
			printf("> Client %s | Outdated version (%llu)\n", m_client_ip.c_str(), version_packet.m_version);

			version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, true);
			m_connection.set_buffer(&version_packet, sizeof version_packet);
			m_connection.send();

			return;
		}

		printf("> Client %s | Updated version\n", m_client_ip.c_str());

		version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, false);
		m_connection.set_buffer(&version_packet, sizeof version_packet);
		m_connection.send();
	}

	// Login Packets
	{
		static std::vector<std::tuple<std::string, uint64_t, uint64_t>> m_users{
			{ "phage", 10382422081949954444, 6941575562263925942 },
			{ "mexican", 2489908105208420549, 11399085311125507402 },
			{ "Ciaran", 10447304709281239008, 4538760847905949811 },
			{ "uzi", 13694325770137587408, 5988022699623831536 },
			{ "gangsta", 5358537683607507742, 16595086355998634058 },
			{ "hacs", 14393557407244641642, 10371841605793258569 },
			{ "denzo", 7646908165282335792, 4134658753895949041 },
			{ "wise", 9853761865339289714, 11395424104966938625 }
		};

		c_login_packet login_packet;
		if (!receive_packet<c_login_packet>(login_packet))
			return;

		auto logged_in = std::any_of(m_users.begin(), m_users.end(), [&login_packet](const std::tuple<std::string, uint64_t, uint64_t>& user)
			{
				auto [username, password, hwid] = user;
				return (strcmp(username.c_str(), login_packet.m_username) == 0 && password == login_packet.m_password && hwid == login_packet.m_hwid);
			});

		if (!logged_in)
		{
			printf("> Client %s | Attempted user: %s pass: %llu hwid: %llu\n", m_client_ip.c_str(), login_packet.m_username, login_packet.m_password, login_packet.m_hwid);

			return;
		}

		printf("> Client %s | Logged into user %s\n", m_client_ip.c_str(), login_packet.m_username);
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
		if (!receive_packet<c_cheat_packet>(cheat_packet))
			return;

		cheat_packet = m_packet_handler.create_cheat_packet("LWJGL", cheat_packet.m_id);
		m_connection.set_buffer(&cheat_packet, sizeof cheat_packet);
		m_connection.send();
	}

	// Send Cheat
	{
		m_connection.set_buffer(crypt_mc_dll, crypt_mc_dll_size);
		m_connection.send();
	}
}
