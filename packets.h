#pragma once

enum class connection_stage : uint8_t
{
	STAGE_WAITING,
	STAGE_LOGIN,
	STAGE_SELECT,
	STAGE_WATCHDOG,
	STAGE_CLOSE,
	STAGE_INVALID
};

enum class packet_type_t : uint64_t
{
	PACKET_VERSION,
	PACKET_LOGIN,
	PACKET_GAMES,
	PACKET_CHEAT,
	PACKET_DATA,
	PACKET_WATCHDOG
};

enum class game_packet_status_t : uint8_t
{
	GAME_INVALID, // used for sending the number of games
	GAME_OFFLINE,
	GAME_NOSUB,
	GAME_DOWN,
	GAME_UPDATING,
	GAME_ONLINE
};

enum class watchdog_packet_status_t : uint8_t
{
	WATCHDOG_KEEPALIVE,
	WATCHDOG_CLOSE,
	WATCHDOG_WARN,
	WATCHDOG_BAN
};

#pragma pack(push, 1) 
struct c_packet
{
	packet_type_t m_type;
	uint64_t      m_key;
	uint64_t      m_send_time;
};

struct c_version_packet : c_packet
{
	uint64_t m_version;
	bool  m_upgrade_required;
};

struct c_login_packet : c_packet
{
	char     m_username[16];
	uint64_t m_password;
	uint64_t m_hwid;
};

struct c_games_packet : c_packet
{
	char				 m_name[64];
	uint64_t			 m_expire;
	uint32_t			 m_version;
	uint8_t				 m_num; // number of games
	game_packet_status_t m_status;
};

struct c_cheat_packet : c_packet
{
	char	 m_window_class[64];
	uint64_t m_cheat_key;
	uint8_t  m_id;
};

struct c_data_packet : c_packet
{
	uint64_t m_len;
	uint8_t  m_buffer[65536];
};

struct c_watchdog_packet : c_packet
{
	watchdog_packet_status_t m_status;
	char					 m_detection_name[64];
};
#pragma pack(pop)

static constexpr uint64_t max_data_len = 65535;

class c_packet_handler
{
	template <typename T>
	void create_base_packet(T& packet, packet_type_t type)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");

		uint64_t key = 0;
		uint8_t* p = reinterpret_cast<uint8_t*>(&key);
		for (auto i = 0; i < 8; i++)
			p[i] = static_cast<uint8_t>(1 + rand() / (RAND_MAX / (255) + 1)); // better randomness than rand() % 255

		packet.m_type = type;
		packet.m_key = key;
		packet.m_send_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
	}


public:
	template <typename T>
	void xor_packet(T& packet)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");
		constexpr static uint8_t packet_offset = 24; // The offset from the beginning of the packet to be xor'd. Type, Key, and Time are skipped

		uint8_t* p = reinterpret_cast<uint8_t*>(&packet.m_key);
		uint8_t* login_p = reinterpret_cast<uint8_t*>(&packet);
		for (size_t i = packet_offset; i < sizeof packet; i++)
			login_p[i] ^= p[i % 8];
	}

	template <typename T>
	void xor_packet(T* packet, size_t size)
	{
		static_assert(std::is_base_of<c_packet, T>::value, "T must derive from c_packet");
		constexpr static uint8_t packet_offset = 24; // The offset from the beginning of the packet to be xor'd. Type, Key, and Time are skipped

		uint8_t* p = reinterpret_cast<uint8_t*>(&packet->m_key);
		uint8_t* login_p = reinterpret_cast<uint8_t*>(packet);
		for (size_t i = packet_offset; i < size; i++)
			login_p[i] ^= p[i % 8];
	}

	template <typename T>
	bool validate_packet(const T& packet)
	{
		if (packet.m_type < packet_type_t::PACKET_VERSION || packet.m_type > packet_type_t::PACKET_WATCHDOG)
			return false;

		if (packet.m_send_time > static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()))
			return false;

		return true;
	}

	c_version_packet create_version_packet(uint64_t version, bool update_required)
	{
		c_version_packet version_packet{};
		create_base_packet(version_packet, packet_type_t::PACKET_VERSION);

		version_packet.m_version = version;
		version_packet.m_upgrade_required = update_required;

		xor_packet(version_packet);

		return version_packet;
	}

	c_login_packet create_login_packet(const std::string& username, uint64_t password, uint64_t hwid)
	{
		c_login_packet login_packet{};
		create_base_packet(login_packet, packet_type_t::PACKET_LOGIN);

		memcpy(&login_packet.m_username, username.data(), sizeof login_packet.m_username);
		login_packet.m_password = password;
		login_packet.m_hwid = hwid;

		xor_packet(login_packet);

		return login_packet;
	}

	c_games_packet create_games_packet(const std::string& name, uint64_t expire, uint32_t version, uint8_t num, game_packet_status_t status)
	{
		c_games_packet games_packet{};
		create_base_packet(games_packet, packet_type_t::PACKET_GAMES);

		memcpy(&games_packet.m_name, name.data(), sizeof games_packet.m_name);
		games_packet.m_expire = expire;
		games_packet.m_version = version;
		games_packet.m_num = num;
		games_packet.m_status = status;

		xor_packet(games_packet);

		return games_packet;
	}

	c_cheat_packet create_cheat_packet(const std::string& window_class, uint8_t id)
	{
		c_cheat_packet cheat_packet{};
		create_base_packet(cheat_packet, packet_type_t::PACKET_CHEAT);

		uint64_t key = 0;
		auto* p = reinterpret_cast<uint8_t*>(&key);
		for (auto i = 0; i < 8; i++)
			p[i] = static_cast<uint8_t>(1 + rand() / (RAND_MAX / (255) + 1)); // better randomness than rand() % 255

		const auto wnd_class = window_class.substr(0, 64);
		strcpy(cheat_packet.m_window_class, wnd_class.c_str());
		cheat_packet.m_cheat_key = key;
		cheat_packet.m_id = id;
	
		xor_packet(cheat_packet);
	
		return cheat_packet;
	}

	c_data_packet* create_data_packet(size_t size, const uint8_t* buffer, size_t& bytes_read)
	{
		c_data_packet* data_packet = new c_data_packet;
		create_base_packet(*data_packet, packet_type_t::PACKET_DATA);
		const auto bytes_to_read = std::min(sizeof data_packet->m_buffer, size - bytes_read);
	
		data_packet->m_len = std::min(size, sizeof data_packet->m_buffer);
		memcpy(data_packet->m_buffer + bytes_read, buffer, bytes_to_read);
		bytes_read += bytes_to_read;
	
		xor_packet(data_packet, sizeof *data_packet);
	
		return data_packet;
	}

	c_watchdog_packet create_watchdog_packet(watchdog_packet_status_t status, const std::string& detection_name = {})
	{
		c_watchdog_packet watchdog_packet{};
		create_base_packet(watchdog_packet, packet_type_t::PACKET_WATCHDOG);

		watchdog_packet.m_status = status;

		const auto dtc_name = detection_name.substr(0, 64);
		if (!dtc_name.empty())
			strcpy(watchdog_packet.m_detection_name, dtc_name.c_str());
		
		return watchdog_packet;
	}
};
