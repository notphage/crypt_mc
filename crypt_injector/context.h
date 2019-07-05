#pragma once

#include <chrono>
#include <iostream>

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

#include "sdk.h"
#include "hash.h"
#include "string.h"
#include "renderer.h"
#include "window.h"
#include "inject.h"
#include "target.h"
#include "connection.h"
#include "client.h"

class c_context
{
public:
	HINSTANCE m_instance = nullptr;
	HWND m_window;
	uint64_t m_version = 1;
	bool m_panic = false;

	c_syscall m_syscall;
	c_inject m_injector;
	c_target m_target;
	c_window m_loader_window;
};

extern c_context ctx;
