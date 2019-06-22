#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#define anti_rin( items )\
 static bool init = true; \
if ( init ) \
{ \
	items; \
	init = false;\
}

#include "hash.h"
#include "string.h"
#include "inject.h"
#include "target.h"
#include "connection.h"

class c_context
{
public:
	c_syscall m_syscall;
	c_inject m_injector;
	c_target m_target;
	c_connection m_connection;
};

extern c_context ctx;
