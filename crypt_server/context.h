#pragma once

#include "pthread.h"

#include <vector>
#include <cstdint>
#include <cstring>
#include <thread>
#include <functional>
#include <queue>
#include <thread>
#include <mutex>
#include <future>
#include <iostream>

#include "thread_pool.h"
#include "console.h"
#include "connection.h"
#include "server.h"

#define anti_rin( items )\
 static bool init = true; \
if ( init ) \
{ \
	items; \
	init = false;\
}

class c_context
{
public:
	std::atomic<uint64_t> m_clients = 0;
	c_console m_console;
};

extern c_context ctx;