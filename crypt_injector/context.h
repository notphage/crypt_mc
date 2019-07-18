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

#define D3D_DEBUG_INFO

#include "sdk.h"
#include "hash.h"
#include "string.h"
#include "util.h"
#include "renderer.h"
#include "gui.h"
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
	bool m_block_keyinput = false;

	float m_frametime = 0.f;

	uint32_t m_screen_w = 320;
	uint32_t m_screen_h = 240;

	c_syscall m_syscall;
	c_inject m_injector;
	c_target m_target;
	c_window m_loader_window;
	std::unique_ptr<c_renderer> m_renderer;
	//c_gui m_gui;
};

extern c_context ctx;
