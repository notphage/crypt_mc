#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <functional>
#include <atomic>
#include <chrono>
#include <optional>
#include <mutex>

#define anti_rin( items )\
 static bool init = true; \
if ( init ) \
{ \
	items; \
	init = false;\
}

typedef unsigned long ulong32_t; // c:
typedef HMODULE module_t;
typedef HANDLE handle_t;
typedef FILE file_t;
typedef HWND hwnd_t;

#include "renderer.h"
#include "font.h"
#include "sdk.h"
#include "memory.h"
#include "math.h"
#include "input.h"
#include "settings.h"
#include "client.h"

enum MC_VERSION
{
	MC_UNKNOWN,
	MC_1710,
	MC_18X,
};

class c_context
{
public:
	HINSTANCE m_instance;
	HWND m_window;
	HGLRC m_rendering_ctx;

	char m_username[32];

	bool m_panic = false;
	bool m_init = false;
	bool m_menu_open = true;
	bool m_menu_closing = false;
	bool m_menu_opening = false;
	bool m_block_keyinput = false;

	LONG m_screen_w = 0;
	LONG m_screen_h = 0;

	std::atomic<MC_VERSION> m_version;
	std::vector<std::unique_ptr<c_feature>> m_features;

	std::shared_ptr<c_renderer> m_renderer;
	std::shared_ptr<c_gui> m_gui;
	c_settings m_settings;
	//c_client m_client;

	std::vector<std::string> m_cfg_list;
	int m_current_cfg{};

	float m_frametime = 0.f;
	std::atomic_bool m_ingame = false;
	std::atomic_bool m_in_chat = false;

	std::atomic<client_flavor> m_client_flavor = VANILLA;

public:
	void determine_version();

	std::shared_ptr<c_class_loader> get_class_loader(JNIEnv*);
	std::shared_ptr<c_game> get_game(JNIEnv*);
};

extern c_context ctx;

namespace hooked
{
	extern int __stdcall swap_buffers(HDC);
	extern long __stdcall get_update(JNIEnv*, jclass);
	extern long __stdcall get_time(JNIEnv*, jclass);
	extern long __stdcall strict_math_atan2(JNIEnv*, jclass);
	extern long long __stdcall wnd_proc(HWND, uint32_t, uint64_t, int64_t);

	extern decltype(&swap_buffers) o_swap_buffers;
	extern decltype(&get_update) o_get_update;
	extern decltype(&get_time) o_get_time;
	extern decltype(&strict_math_atan2) o_strict_math_atan2;
	extern decltype(&wnd_proc) o_wnd_proc;
}
