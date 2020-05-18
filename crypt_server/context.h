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
#include <fstream>

#include <openssl/crypto.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/engine.h>
#include <openssl/conf.h>

#include "thread_pool.h"
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
	std::unique_ptr<c_server> m_server;
	uint64_t m_required_version = 3;
};

extern c_context ctx;