#include <Windows.h>
#include <cstdint>
#include <vector>
#include <functional>
#include <mutex>
#include <conio.h>
#include <atomic>

#include "../shared_mem.h"
#include "../shared_queue.h"

c_mem_handler g_mem_handler;

constexpr auto start_cookie = 0xDEADBEEFC0DE;

int main()
{
	const c_shared_mem_queue shared_mem_queue("Spotify", 0x100000, c_shared_mem_queue::mode::server);
	
	printf("Test server start\n");
	_getch();

	static auto fnv1a_u64 = [](uint64_t cookie) -> uint64_t
	{
		uint64_t ret = 0xcbf29ce484222325;
		auto* p = reinterpret_cast<uint8_t*>(&cookie);
		for (auto i = 0; i < 8; i++)
		{
			ret ^= p[i];
			ret *= 0x100000001b3;
		}

		return ret;
	};

	auto init_packet = g_mem_handler.create_init_mem_packet("phage", fnv1a_u64(start_cookie));

	const mem_message_t init_msg(reinterpret_cast<uint8_t*>(&init_packet), sizeof init_packet);
	if (!shared_mem_queue.push_message(init_msg))
		printf("Failed to push messaged\n");
	else
		printf("Init message pushed\n");

	uint64_t expected_cookie = fnv1a_u64(fnv1a_u64(start_cookie));
	for (int i = 0; i < 3; i++)
	{
		mem_message_t msg{};
		if (!shared_mem_queue.wait_for_message())
		{
			printf("Ping timed out\n");
			break;
		}

		if (!shared_mem_queue.pop_message(msg))
		{
			printf("Failed to pop message\n");
			break;
		}
		
		printf("Ping popped\n");

		const auto ping_packet = reinterpret_cast<c_mem_ping_packet*>(msg.m_data.data());
		if (!g_mem_handler.validate_mem_packet(*ping_packet))
		{
			printf("Ping packed invalid\n");
			break;
		}
		g_mem_handler.xor_mem_packet(*ping_packet);

		printf("num: %llu\n", ping_packet->m_hash);
		if (ping_packet->m_hash != expected_cookie)
			printf("Cookie was not valid\n");

		expected_cookie = fnv1a_u64(ping_packet->m_hash);
	}

	auto close_packet = g_mem_handler.create_close_mem_packet(false);

	const mem_message_t close_msg(reinterpret_cast<uint8_t*>(&close_packet), sizeof close_packet);
	if (!shared_mem_queue.push_message(close_msg))
		printf("Failed to push messaged\n");
	else
		printf("Init message pushed\n");

	_getch();
}
