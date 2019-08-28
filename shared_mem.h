#pragma once

enum class shared_mem_stage : uint8_t
{
	STAGE_WAITING,
	STAGE_CREATE,
	STAGE_CLOSE,
	STAGE_FORCECLOSE,
	STAGE_INVALID
};

enum class mem_type_t : uint64_t
{
	MEM_INIT,
	MEM_HOOK,
	MEM_PING
};

#pragma pack(push, 1) 
struct c_mem_packet
{
	mem_type_t    m_type;
	uint64_t      m_key;
	uint64_t      m_send_time;
};

struct c_mem_init_packet : c_mem_packet
{
	char m_username[64];
	uint64_t m_cookie;
};

struct c_mem_hook_packet : c_mem_packet
{
	uintptr_t m_hook_addr;
};

struct c_mem_ping_packet : c_mem_packet
{
	uint64_t m_hash;
};
#pragma pack(pop)

class c_mem_handler
{
	template <typename T>
	void create_base_mem_packet(T& packet, mem_type_t type)
	{
		static_assert(std::is_base_of<c_mem_packet, T>::value, "T must derive from c_mem_packet");

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
	void xor_mem_packet(T& packet)
	{
		static_assert(std::is_base_of<c_mem_packet, T>::value, "T must derive from c_mem_packet");
		constexpr static uint8_t packet_offset = 24; // The offset from the beginning of the packet to be xor'd. Type, Key, and Time are skipped

		uint8_t* p = reinterpret_cast<uint8_t*>(&packet.m_key);
		uint8_t* login_p = reinterpret_cast<uint8_t*>(&packet);
		for (size_t i = packet_offset; i < sizeof packet; i++)
			login_p[i] ^= p[i % 8];
	}

	template <typename T>
	bool validate_mem_packet(const T& packet)
	{
		static_assert(std::is_base_of<c_mem_packet, T>::value, "T must derive from c_mem_packet");
		
		if (packet.m_type < mem_type_t::MEM_INIT || packet.m_type > mem_type_t::MEM_PING)
			return false;

		if (packet.m_send_time > static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()))
			return false;

		return true;
	}

	c_mem_init_packet create_init_mem_packet(const char* username, uint64_t cookie)
	{
		c_mem_init_packet init_mem_packet{};
		create_base_mem_packet(init_mem_packet, mem_type_t::MEM_INIT);

		strcpy_s(init_mem_packet.m_username, sizeof init_mem_packet.m_username, username);
		init_mem_packet.m_cookie = cookie;

		xor_mem_packet(init_mem_packet);

		return init_mem_packet;
	}

	c_mem_hook_packet create_hook_mem_packet(uintptr_t hook_addr)
	{
		c_mem_hook_packet hook_mem_packet{};
		create_base_mem_packet(hook_mem_packet, mem_type_t::MEM_HOOK);

		hook_mem_packet.m_hook_addr = hook_addr;

		xor_mem_packet(hook_mem_packet);

		return hook_mem_packet;
	}

	c_mem_ping_packet create_ping_mem_packet(uint64_t last_hash)
	{
		c_mem_ping_packet ping_mem_packet{};
		create_base_mem_packet(ping_mem_packet, mem_type_t::MEM_PING);

		ping_mem_packet.m_hash = last_hash;

		xor_mem_packet(ping_mem_packet);

		return ping_mem_packet;
	}
};
