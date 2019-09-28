#pragma once

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include <iostream>
#include <queue>
#include <thread>
#include <atomic>
#include <mutex>

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "crypt32")
#pragma comment(lib, "ws2_32.lib")

#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)    // ntsubauth

#define anti_rin( items )\
 static bool init = true; \
if ( init ) \
{ \
	items; \
	init = false;\
}

#include "asmlib/asmlib.h"
#include "asmlib/asmlibran.h"
#include "lazy_import.h"

#include "sdk.h"
#include "hash.h"
#include "string.h"
#include "util.h"
#include "hwid.h"
#include "renderer.h"
#include "gui.h"
#include "window.h"
#include "memory.h"
#include "connection.h"
#include "protection.h"
#include "client.h"

class c_context
{
public:
	HINSTANCE m_instance = nullptr;
	HWND m_window;
	uint64_t m_version = 2;
	int32_t m_selected_cheat = -1;
	bool m_block_keyinput = false;
	std::vector<std::byte> m_buffer;
	std::string m_window_class;
	std::string m_username;

	std::atomic_bool m_panic = false;
	std::atomic_bool m_watchdog = false;

	float m_frametime = 0.f;

	uint32_t m_screen_w = 320;
	uint32_t m_screen_h = 240;

	c_syscall m_syscall;
	c_window m_loader_window;
	c_hwid m_hwid;
	c_client m_client;
	std::unique_ptr<c_renderer> m_renderer;
	std::vector<c_game_entry> m_game_list;
	std::mutex m_game_list_mutex;
};

extern c_context ctx;
