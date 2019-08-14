#include "context.h"
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

c_context ctx;

int32_t __stdcall WinMain(HINSTANCE instance, HINSTANCE, char*, int32_t)
{
	if (!ctx.m_syscall.init())
		return 0;

	ctx.m_instance = instance;
	ctx.m_loader_window.create();

	MSG msg{};
	LI_FN(PeekMessageA).cached()(&msg, nullptr, 0, 0, PM_REMOVE);
	while (msg.message != WM_QUIT && !ctx.m_panic && !ctx.m_watchdog)
	{
		if (LI_FN(PeekMessageA).cached()(&msg, ctx.m_window, 0, 0, PM_REMOVE))
		{
			LI_FN(TranslateMessage).cached()(&msg);
			LI_FN(DispatchMessageA).cached()(&msg);
		}
		else
		{
			// proper frametime
			static auto old = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			const float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
			old = now;

			ctx.m_frametime = delta / 1000.f;

			ctx.m_loader_window.render();
		}
	}

	ctx.m_loader_window.end();

	if (ctx.m_watchdog)
	{
		while (ctx.m_watchdog)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	}

	return 0;
}