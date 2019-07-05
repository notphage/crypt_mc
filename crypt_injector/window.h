#pragma once

#include "input.h"

class c_window
{
	HWND m_window;

	static long long __stdcall wnd_proc(HWND hwnd, uint32_t msg, uint64_t uparam, int64_t param);
public:
	void create();
};