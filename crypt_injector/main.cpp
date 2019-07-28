#include "context.h"
#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

c_context ctx;

int32_t __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, int32_t cmd_show)
{
	ctx.m_syscall.init();
	ctx.m_instance = instance;
	ctx.m_loader_window.create();

	MSG msg;
	PeekMessageA(&msg, ctx.m_window, 0, 0, PM_REMOVE);
	while (msg.message != WM_QUIT)
	{
		if (PeekMessageA(&msg, ctx.m_window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}
		else
		{
			// proper frametime
			static auto old = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
			old = now;

			ctx.m_frametime = delta / 1000.f;

			ctx.m_loader_window.render();
		}
	}

	ctx.m_loader_window.end();

	return 0;
}