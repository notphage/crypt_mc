#pragma once

#include "../shared_mem.h"
#include "../shared_queue.h"
#include <thread>

class c_client
{
	std::atomic<shared_mem_stage> m_shared_mem_stage;
	std::thread m_shared_mem_thread;

	c_shared_mem_queue m_mem_queue;
	c_mem_handler m_mem_handler;

	uint64_t m_cookie = 0;

	void run_shared_mem();
public:
	c_client()
		: m_shared_mem_stage(shared_mem_stage::STAGE_CREATE), m_shared_mem_thread(std::thread(&c_client::run_shared_mem, this)),
		 m_mem_queue(xors("Spotify"), 0x100000, c_shared_mem_queue::mode::client)
	{ }

	~c_client()
	{
		m_shared_mem_thread.join();
	}

	void set_shared_mem_stage(shared_mem_stage stage)
	{
		m_shared_mem_stage = stage;
	}

	shared_mem_stage get_shared_mem_stage() const
	{
		return m_shared_mem_stage;
	}
};
