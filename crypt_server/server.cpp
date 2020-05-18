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
#include <signal.h>

c_server::c_server(int16_t port)
	: m_port(port)
{
	safe_cout << "> Server | Threads: " << std::thread::hardware_concurrency() << " Port: " << m_port << "\n";
}

c_server::~c_server()
{
	BIO_free_all(m_cbio);
	BIO_free_all(m_kbio);

	SSL_CTX_free(m_ssl_ctx);
}

void c_server::init_tls()
{
	// Internal SSL init functions
	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	// Authentication
	m_ssl_ctx = SSL_CTX_new(TLS_server_method());
	if (!m_ssl_ctx)
		throw std::runtime_error("Couldn't create SSL/TLS context\n");

	if (SSL_CTX_set_min_proto_version(m_ssl_ctx, TLS1_2_VERSION) == 0)
		throw std::runtime_error("Couldn't set minimum version to TLS 1.2");

	m_cbio = BIO_new_mem_buf(crypt_cert, crypt_cert_size);
	X509* cert = PEM_read_bio_X509(m_cbio, nullptr, nullptr, nullptr);
	if (cert == nullptr)
		throw std::runtime_error("Couldn't load certificate\n");
	SSL_CTX_use_certificate(m_ssl_ctx, cert);

	m_kbio = BIO_new_mem_buf(crypt_pem, crypt_pem_size);
	RSA* rsa = PEM_read_bio_RSAPrivateKey(m_kbio, nullptr, nullptr, nullptr);
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
	constexpr uint64_t infrac_cooldown = 24 * 60 * 60;
	constexpr uint32_t max_infractions = 3;
	const uint64_t cur_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

	if (m_ip_infractions.empty())
		return false;

	m_ip_infractions.erase(std::remove_if(m_ip_infractions.begin(), m_ip_infractions.end(), [&infrac_cooldown, &cur_time](auto&& ip_infrac)
		{
			return cur_time - ip_infrac.m_infraction_time > infrac_cooldown;
		}), m_ip_infractions.end());

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
	srand((unsigned int)time(nullptr));
	init_tls();
	bind_socket();

	int client_socket = -1, c = sizeof(struct sockaddr_in);
	struct sockaddr_in client_addr;

	while ((client_socket = accept(m_socket_desc, reinterpret_cast<struct sockaddr*>(&client_addr), reinterpret_cast<socklen_t*>(&c))))
	{
		if (is_ip_banned(client_addr))
		{
			if (shutdown(client_socket, SHUT_RDWR) < 0)
				close(client_socket);

			continue;
		}
		
		struct timeval tv;
		tv.tv_sec = 15;
		tv.tv_usec = 0;
		setsockopt(client_socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&tv), sizeof tv);
		setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&tv), sizeof tv);
		sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);

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
				return client->is_disconnected() || !client->is_running();
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

	if (SSL_accept(m_ssl) <= 0)
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
			
			m_is_running = false;
			return;
		}

		if (ctx.m_required_version > version_packet.m_version)
		{
			safe_cout << "> Client " << m_client_ip << " | Outdated version (" << version_packet.m_version << ")\n";

			version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, true);
			send_packet(version_packet);

			m_is_running = false;
			return;
		}

		safe_cout << "> Client " << m_client_ip << " | Updated version\n";

		version_packet = m_packet_handler.create_version_packet(ctx.m_required_version, false);

		if (send_packet<c_version_packet>(version_packet) != CHPS_VALID)
		{
			m_is_running = false;
			return;
		}
	}

	// Login Packets
	{
		m_stage = CHS_LOGIN;
		
		static std::vector<std::tuple<std::string, uint64_t, uint64_t>> m_users{
			{ "phage", 10382422081949954444, 5207655261874637113 },
			{ "mexican", 2489908105208420549, 3016620873829748165 },
			{ "Ciaran", 10447304709281239008, 13088742455535597796 },		 //
			{ "uzi", 13694325770137587408, 7665409662216235665 },
			{ "gangsta", 5358537683607507742, 18047120074447871436 },		 //
			{ "denzo", 7646908165282335792, 2493595977312826428 },
			{ "wise", 9853761865339289714, 650782761150847554 },			 //
			{ "_Rtinox", 1308268265119556128, 2668412589270920540 },		 //
			{ "nomo", 17123985505359595210, 17660965088890772486 },			 //
			{ "Nash", 15055877905642066443, 15876347534175914011 },			 //
			{ "emprah", 17995296015186350121, 12411031163070736735 },		 //
			{ "Skilled", 6993857605014467185, 9015217248360126449 },
			{ "alexandru", 10695783107824520511, 13091128279247074244 },
			{ "SnowFlake", 16915402360579880554, 9299927410050693940 },		 //
			{ "Zach", 1836054350097038111, 5811364667087097117 },
			{ "1300s", 8471790438192775144, 2717312826946601474 },
			{ "aris", 14781963634558499306, 10141932124092029056 },			 //
			{ "Rust", 3392171804832091563, 1916659933398714382 },
			{ "Scompey", 239235789231852688, 7122098932018716504 },
			{ "Yersi", 6266182343525247170, 15279275253294033247 },
			{ "mishtavishta", 524839665675923563, 8450240913602343789 },
			{ "SR", 12920697777794270736, 17559757935203518501 },			 //
			{ "kyle", 11367569745228157569, 16033228892697785651 },
			{ "daxe", 5380418998039192649, 13115787265030824506 },
			{ "Mango",  12638153115695167455, 1028821631945514990 },		 //
			{ "spo", 15055877905642066443, 16314294747567816438 },
			{ "Arcazze", 8051186676514505564, 16833326617975758299 },
			{ "tw1sted_", 777194763440543867, 13262703185437387377 },		 //
			{ "DBOG", 5231375840648763331, 13838107329980287696 },			 //
			{ "ImUnknown99", 15087550580990042902, 11037750867168822815 },
			{ "sconald", 677587761371028860, 2806498963281756937 },			 //
			{ "xuanyu123", 13994120633482333186, 6847807079912293922 },		 //
			{ "ignGemit", 4457376732273871185, 797720607300673121 },		 //
			{ "Alfie", 6682312334506917563, 4612275323266429992 },
			{ "Crimzoh", 7785559992960956338, 2744311021037323276 },
			{ "fatloser", 1230838956535179287, 18438506710989643322 },		 //
			{ "MacKyler", 8984238175616723189, 12305414478760031497 },		 //
			{ "StuxNet", 18174286778672980884, 1513838039731587410 },
			{ "madimadmad", 12584405072435123421, 1122971527930609280 },	 //
			{ "Kate", 390058373728126496, 4383821432950351071 },			 //
			{ "inhonia", 11337685419364724121, 7991615946164949672 },
			{ "Red", 9712162055907340288, 17292056422940887055 },
			{ "jhyung", 15267160324146910984, 11302538933756578248 },		 //
			{ "Virose", 3217573911261357502, 15026689781495521049 },
			{ "Cead", 12475560696163484049, 9604301214274457888 },
			{ "noof", 17003909983449241752, 14510085206031467540 },			 //
			{ "LevensLes", 11575185211461752072, 4305117490831805832 },		 //
			{ "solo", 10738406412815510469, 3614060349151523510 },
			{ "Atomix", 18303482491684880771, 9126155783666581749 },		 //
			{ "adamdrex", 11709734982865981770, 17179495266235867336 },		 //
			{ "g1ock", 7345127029555614684, 3464896207616951624 },
			{ "Kerman", 6357163824747178457, 6321025916876967040 },			 //
			{ "athing", 17823852358385833567, 15686858299307463971 },
			{ "Saussy", 10752424429693966821, 17574027574713474782 },
			{ "pickleback", 873878496268894415, 3270259055479825584 },		 //
			{ "lira", 491748478205758166, 5205900878377158646 },
			{ "mahbray", 836147050829168777, 17959495278156982599 },
			{ "ocewot", 9124344186921853259, 17423107220615943456 },
			{ "nxvkk", 7442889234024777451, 18006360824138659369 },
			{ "Cleid", 5183631825819480699, 14349548185503922102 },			 //
			{ "Firerider", 3203095425716611200, 166468611889752005 },
			{ "aimgallery", 18134069896550538631, 4065174568687766600 },
			{ "tdmp2", 3535481208941659031, 6632291406566432241 },
			{ "hulu", 1452776112977783515, 17481861083655247469 },			 //
			{ "Faasty", 2334650520560576728, 2792308857356692355 },			 //
			{ "Autron", 18100314209343369963, 13853506490689054178 },		 //
			{ "Coun", 16833490774929489357, 9137490567002502329 },
			{ "Nelier", 8227787477923233890, 12397428229078966401 },		 //
			{ "ewthan", 11595853088393130140, 9092728580187992081 },
			{ "scelxste", 9672812331812111327, 4790401959680621653 },		 //
			{ "along", 18406822085346311909, 9514495867462295596 },
			{ "XuanYu123", 15032361248381345566, 8078177903852438455 },
			{ "Stayorx", 533684561528333532, 17756270274640695976 },		 //
			{ "xallypvp420", 1274047885066180356, 15501467000086554122 },
			{ "Mouka", 18109390471197889793, 6835931714683244355 },			 //
			{ "hxsank", 7895894271594868644, 241275951080001654 },
			{ "Spookay", 3370795320672299193, 10577922486796688377 },
			{ "Peti", 9647123753609417832, 17170653678748200876 },
			{ "julie", 11463234737593600444, 11850799282932994524 },
			{ "ajvstheworld", 14259834551808341602, 11843833999515529023 },
			{ "congo", 10449447729407531838, 9668904119876533158 },
			{ "josh", 12078562124763649800, 9125336484697132037 },
			{ "edge", 8068852697002483627, 12831525087405446060 },
			{ "Drastically", 15706723293724439533, 6855550330694214400 },
			{ "MinecraftHaxor", 13084894519989705724, 4631370490399380227 },
			{ "BadSlipperr", 1500365963061416657, 18011434506695941628 },
			{ "bap", 9672812331812111327, 12600972553801052227 },
			{ "TopTrash", 9392992003679640907, 7435812755760161042 },
			{ "Exmated", 6504948629669439413, 1172688444867117477 },
			{ "Torrova", 11893061839897935642, 12024398630433738087 },
			{ "hacks", 15698637688762441189, 14223465993694777177 },
			{ "ariuu", 12321655269417445881, 18360884870071357183 },
			{ "matrixxas", 1122264786951176963, 15816752089032620445 },
			{ "Cleid", 5183631825819480699, 2612829799356865093 },
			{ "Fry", 5637900669906283811, 1902385742520424945 },
			{ "matrixxas", 1122264786951176963, 17494144794684390453 },
			{ "LukezasZ", 11326791262334998069, 17318693401832416558 },
			{ "Jooqi", 4917749041364318085, 7003547806571075932 },
			{ "csgopro46", 13272689621294344530, 10546729789537911275 },
			{ "bladee", 2233010070871558968, 1599484750772555127 },
			{ "ariuu", 12321655269417445881, 9538383052831178226 },
			{ "sleep", 16645271841666678365, 15956636201786254416 },
			{ "ssnoverify", 4688260690687200065, 2651656441012565992 },
			{ "J0hnny", 7103762199107710453, 11518383997500831451 },
			{ "WMZ", 3946336520710475428, 13326604059684606583 },
			{ "vape", 17888977531490432847, 6476144646079859600 },
			{ "theyob", 1652002062593564680, 8444521818992320546 },
			{ "privileged", 5921117170507710309, 5834534029578187128 },
			{ "pew", 1053800740257648375, 8862983021015326752 }
		};

		c_login_packet login_packet;
		if (const auto ret = handle_packet<c_login_packet>(login_packet); ret != CHPS_VALID)
		{
			if (ret == CHPS_INVALID)
				ctx.m_server->add_infraction(shared_from_this());

			m_is_running = false;
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
			send_packet<c_login_packet>(login_packet);

			m_is_running = false;
			return;
		}

		safe_cout << "> Client " << m_client_ip << " | Logged into user " << login_packet.m_username << "\n";

		m_packet_handler.xor_packet(login_packet);

		if (send_packet<c_login_packet>(login_packet) != CHPS_VALID)
		{
			m_is_running = false;
			return;
		}
	}

	// Game Packets
	{
		c_games_packet game_packet = m_packet_handler.create_games_packet("", 0, 0, (uint8_t)m_games.size(), game_packet_status_t::GAME_INVALID);
		
		if (send_packet<c_games_packet>(game_packet) != CHPS_VALID)
		{
			m_is_running = false;
			return;
		}

		for (auto&& game : m_games)
		{
			game_packet = m_packet_handler.create_games_packet(game.m_name, 1, 30, (uint8_t)m_games.size(), game.m_status);

			if (send_packet<c_games_packet>(game_packet) != CHPS_VALID)
			{
				m_is_running = false;
				return;
			}
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

			m_is_running = false;
			return;
		}

		cheat_packet = m_packet_handler.create_cheat_packet("LWJGL", cheat_packet.m_id);

		if (send_packet<c_cheat_packet>(cheat_packet) != CHPS_VALID)
		{
			m_is_running = false;
			return;
		}
	}

	// Send Cheat
	{
		if (m_connection.send((void*)crypt_mc_dll, crypt_mc_dll_size) == -1)
		{
			m_is_running = false;
			return;
		}
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
				safe_cout << "> Client " << m_client_ip << " | Warned for " << watchdog_packet.m_detection_name << "\n";

				ctx.m_server->add_infraction(shared_from_this());
				connected = false;
				break;
			}
			
			case watchdog_packet_status_t::WATCHDOG_BAN:
			{
				// TODO: Add REST API communication to ban player
				safe_cout << "> Client " << m_client_ip << " | Banned for " << watchdog_packet.m_detection_name << "\n";

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
	m_is_running = false;
}
