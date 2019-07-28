#pragma once

#include "input.h"

class c_window
{
	static long long __stdcall wnd_proc(HWND hwnd, uint32_t msg, uint64_t uparam, int64_t param);

	c_gui m_gui;
public:
	void create();
	void render();
	void end();

	c_gui& get_gui()
	{
		return m_gui;
	}
};