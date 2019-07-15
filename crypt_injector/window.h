#pragma once

#include "input.h"

class c_window
{
	std::unique_ptr<renderer> m_renderer;

	static long long __stdcall wnd_proc(HWND hwnd, uint32_t msg, uint64_t uparam, int64_t param);

public:
	c_window()
		: m_renderer(std::make_unique<renderer>(8192))
	{ }

	void create();
	void render();
	void end();
};