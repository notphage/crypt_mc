#include "context.h"
#include "cheat.h"
#include "cert.h"
#include "key.h"
#include "safe_cout.h"

#include <sys/socket.h>
#include <unistd.h>    //write
#include <cmath>
#include <tuple>
#include <algorithm>

c_server::c_server(int16_t port)
	: m_port(port)
{
	safe_cout << "> Server | Threads: " << std::thread::hardware_concurrency() << " Port: " << m_port << "\n";
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

	safe_cout << "> SSL | Access granted\n";
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
	server.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY; //inet_addr("127.0.0.1");
	server.sin_port = htons(8123);

	//Bind
	if (bind(m_socket_desc, reinterpret_cast<struct sockaddr*>(&server), sizeof(server)) < 0)
		throw std::runtime_error("Could not bind socket");

	//Listen
	listen(m_socket_desc, 3);

	safe_cout << "> Server | Initialized successfully\n";
}

bool c_server::add_infraction(const std::shared_ptr<c_client_handler>& client)
{
	constexpr uint64_t infrac_cooldown = 24 * 60 * 60;
	constexpr uint32_t max_infractions = 3;

	const uint64_t cur_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	const uint32_t client_ip = client->get_sock_addr().sin_addr.s_addr;

	std::lock_guard<std::mutex> lock_guard(m_ip_infractions_mutex);
	
	bool new_infrac = true;
	if (!m_ip_infractions.empty())
	{
		m_ip_infractions.erase(std::remove_if(m_ip_infractions.begin(), m_ip_infractions.end(), [&infrac_cooldown, &cur_time](auto&& ip_infrac)
			{
				return cur_time - ip_infrac.m_infraction_time > infrac_cooldown;
			}), m_ip_infractions.end());
		
		for (auto&& ip_infrac : m_ip_infractions)
		{
			if (ip_infrac.m_ip != client_ip)
				continue;

			new_infrac = false;

			ip_infrac.m_infraction_count++;
			ip_infrac.m_infraction_time = cur_time;
			if (ip_infrac.m_infraction_count >= max_infractions)
			{
				// ip ban for 24 hours

				safe_cout << "> Server | Banned IP blocked\n";
				return false;
			}
		}
	}

	if (new_infrac)
	{
		m_ip_infractions.push_back({ client_ip,  1, cur_time });
		safe_cout << "> Server | Infraction added\n";

		return true;
	}

	safe_cout << "> Server | Infraction modified\n";
	return true;
}

bool c_server::is_ip_banned(const sockaddr_in& client)
{
	const uint32_t client_ip = client.sin_addr.s_addr;
	constexpr uint32_t max_infractions = 3;
	const uint64_t cur_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	if (m_ip_infractions.empty())
		return false;

	for (auto&& ip_infrac : m_ip_infractions)
	{
		if (ip_infrac.m_ip == client_ip && ip_infrac.m_infraction_count >= max_infractions)
		{
			// reset the infraction time
			ip_infrac.m_infraction_time = cur_time;

			safe_cout << "> Server | Banned ip tried to connect\n";
			return true;
		}
	}
	
	return false;
}

void c_server::run()
{
	srand(time(nullptr));
	init_tls();
	bind_socket();

	int client_socket = -1, c = sizeof(struct sockaddr_in);
	struct sockaddr_in client_addr;

	while ((client_socket = accept(m_socket_desc, reinterpret_cast<struct sockaddr*>(&client_addr), reinterpret_cast<socklen_t*>(&c))))
	{
		if (is_ip_banned(client_addr))
		{
			close(m_socket_desc);
			continue;
		}
		
		struct timeval tv;
		tv.tv_sec = 15;
		tv.tv_usec = 0;
		setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&tv), sizeof tv);
		setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof tv);

		static char ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);

		auto client_handler = std::make_shared<c_client_handler>(ip_str, client_socket, client_addr);
		const auto ret = client_handler->initialize(m_ssl_ctx);

		if (ret == CHI_SUCCESS)
		{
			m_clients.push_back(std::move(client_handler));
			m_clients.back()->run();

			safe_cout << "> Server | Client " << ip_str << " accepted\n";
		}
		else
		{
			safe_cout << "> Server | Client " << ip_str << " rejected for " << ret << "\n";
			add_infraction(client_handler);
		}

		m_clients.erase(std::remove_if(m_clients.begin(), m_clients.end(), [](auto&& client)
			{
				return client->is_disconnected();
			}), m_clients.end());
	}

	if (client_socket < 0)
		throw std::runtime_error("Accept Failed");

	close(m_socket_desc);
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
	m_thread.detach();
}

void c_client_handler::handle_client()
{
	// Version Packets
	{
		m_stage = CHS_VERSION;
		
		c_version_packet version_packet;
		if (const auto ret = handle_packet<c_version_packet>(version_packet); ret != CHPS_VALID)
		{
			if (ret == CHPS_INVALID)
				ctx.m_server->add_infraction(shared_from_this());
			
			return;
		}

		if (ctx.m_required_version > version_packet.m_version)
		{
			safe_cout << "> Client " << m_client_ip << " | Outdated version (" << version_packet.m_version << ")\n";

			version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, true);
			m_connection.set_buffer(&version_packet, sizeof version_packet);
			m_connection.send();

			return;
		}

		safe_cout << "> Client " << m_client_ip << " | Updated version\n";

		version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, false);
		m_connection.set_buffer(&version_packet, sizeof version_packet);
		m_connection.send();
	}

	// Login Packets
	{
		m_stage = CHS_LOGIN;
		
		static std::vector<std::tuple<std::string, uint64_t, uint64_t>> m_users{
			{ "phage", 10382422081949954444, 18005380739928406968 },
			{ "mexican", 2489908105208420549, 17383418848068447227 },
			{ "Ciaran", 10447304709281239008, 9093044100942767960 },
			{ "uzi", 13694325770137587408, 10889602171640250617 },
			{ "gangsta", 5358537683607507742, 16595086355998634058 },
			{ "hacs", 14393557407244641642, 10371841605793258569 },
			{ "denzo", 7646908165282335792, 4134658753895949041 },
			{ "wise", 9853761865339289714, 11395424104966938625 },
			{ "_Rtinox", 1308268265119556128, 6138202367498348673 },
			{ "nomo", 17123985505359595210, 4670644444049306433 },
			{ "Nash", 15055877905642066443, 15876347534175914011 },
			{ "emprah", 17995296015186350121, 12684092619770610821 },
			{ "Skilled", 6993857605014467185, 12131543493270600539 },
			{ "alexandru", 3620744427807918383, 5154951702442138794 },
			{ "NefariousUser", 16915402360579880554, 9729003570243262152 },
			{ "Zach", 1836054350097038111, 6503586793295444129 },
			{ "1300s", 8471790438192775144, 17327406942188991615 },
			{ "aris", 14781963634558499306, 10141932124092029056 },
			{ "Rust", 3392171804832091563, 13144256587507604925 },
			{ "Scompey", 239235789231852688, 9386242540668891877 },
			{ "Yersi", 6266182343525247170, 13751237738621815046 },
			//{ "mishtavishta", 524839665675923563, 18383812991739346673 },
			{ "SR", 12920697777794270736, 17559757935203518501 },
			{ "kyle", 11367569745228157569, 10402789832664191440 },
			{ "toby", 5380418998039192649, 1374052520093222915 },
			{ "Mango",  12638153115695167455, 11498898546036171723 }
		};

		c_login_packet login_packet;
		if (const auto ret = handle_packet<c_login_packet>(login_packet); ret != CHPS_VALID)
		{
			if (ret == CHPS_INVALID)
				ctx.m_server->add_infraction(shared_from_this());

			return;
		}

		login_packet.m_status = login_packet_status_t::LOGIN_INVALID;
		for (auto&& user : m_users)
		{
			auto [username, password, hwid] = user;

			if (strcmp(username.c_str(), login_packet.m_username) != 0)
				continue;

			if (password != login_packet.m_password)
				continue;

			if (hwid != login_packet.m_hwid)
			{
				login_packet.m_status = login_packet_status_t::LOGIN_MISMATCH;
				continue;
			}

			login_packet.m_status = login_packet_status_t::LOGIN_VALID;
			break;
		}

		if (login_packet.m_status != login_packet_status_t::LOGIN_VALID)
		{
			safe_cout << "> Client " << m_client_ip << " | Attempted user: " << login_packet.m_username << " pass: " << login_packet.m_password << " hwid: " << login_packet.m_hwid << "\n";

			m_packet_handler.xor_packet(login_packet);
			m_connection.set_buffer(&login_packet, sizeof login_packet);
			m_connection.send();

			return;
		}

		safe_cout << "> Client " << m_client_ip << " | Logged into user " << login_packet.m_username << "\n";

		m_packet_handler.xor_packet(login_packet);
		m_connection.set_buffer(&login_packet, sizeof login_packet);
		m_connection.send();
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

	// Game Selection
	c_cheat_packet cheat_packet;
	{
		m_stage = CHS_SELECTION;
		
		if (const auto ret = handle_packet<c_cheat_packet>(cheat_packet); ret != CHPS_VALID)
		{
			if (ret == CHPS_INVALID)
				ctx.m_server->add_infraction(shared_from_this());

			return;
		}

		cheat_packet = m_packet_handler.create_cheat_packet("LWJGL", cheat_packet.m_id);
		m_connection.set_buffer(&cheat_packet, sizeof cheat_packet);
		m_connection.send();
	}

	// Send Cheat
	{
		m_connection.set_buffer(crypt_mc_dll, crypt_mc_dll_size);
		m_connection.send();
	}

	m_stage = CHS_WATCHDOG;
	bool connected = true;
	while (connected)
	{
		c_watchdog_packet watchdog_packet;
		if (const auto ret = handle_packet<c_watchdog_packet>(watchdog_packet); ret != CHPS_VALID)
		{
			if (ret == CHPS_INVALID)
				ctx.m_server->add_infraction(shared_from_this());
	
			break;
		}
	
		switch (watchdog_packet.m_status)
		{
			case watchdog_packet_status_t::WATCHDOG_KEEPALIVE:
			{
				break;
			}
			
			case watchdog_packet_status_t::WATCHDOG_CLOSE:
			{
				connected = false;
				break;
			}
			
			case watchdog_packet_status_t::WATCHDOG_WARN:
			{
				ctx.m_server->add_infraction(shared_from_this());
				connected = false;
				break;
			}
			
			case watchdog_packet_status_t::WATCHDOG_BAN:
			{
				// TODO: Add REST API communication to ban player
				connected = false;
				break;
			}
			
			default:
			{
				connected = false;
				break;
			}
		}
		
	}

	m_connection.disconnect();
	m_disconnected = true;
}
