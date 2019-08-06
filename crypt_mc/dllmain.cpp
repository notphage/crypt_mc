#include "context.h"
#include "header.h"
#include "MinHook.h"

#include <winternl.h>

c_context ctx;

// to get rid of a string in microsoft crt
static void __cdecl std::_Xlength_error(const char* error) {}

void unload()
{
	auto gl_disable = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_GL11_nglDisable"));
    auto get_time = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_WindowsSysImplementation_nGetTime"));

	MH_RemoveHook(SwapBuffers);
	MH_RemoveHook(gl_disable);
    MH_RemoveHook(get_time);

	SetWindowLongPtrA(ctx.m_window, -4, (long long)hooked::o_wnd_proc);

	MH_Uninitialize();
}

void hack(HINSTANCE bin)
{
	// store module base
	ctx.m_instance = bin;

	// parse header for user info and some interfaces
	parse_header(uintptr_t(bin));

#ifdef TESTBUILD
	printf(" /$$$$$$$                                     /$$   /$$ /$$                                     /$$   /$$                     /$$      \n");
	printf("| $$__  $$                                   | $$$ | $$|__/                                    | $$  | $$                    | $$      \n");
	printf("| $$  \\ $$  /$$$$$$   /$$$$$$$ /$$$$$$$      | $$$$| $$ /$$  /$$$$$$   /$$$$$$   /$$$$$$       | $$  | $$  /$$$$$$   /$$$$$$ | $$   /$$\n");
	printf("| $$$$$$$  /$$__  $$ /$$_____//$$_____/      | $$ $$ $$| $$ /$$__  $$ /$$__  $$ |____  $$      | $$$$$$$$ /$$__  $$ /$$__  $$| $$  /$$/\n");
	printf("| $$__  $$| $$  \\ $$|  $$$$$$|  $$$$$$       | $$  $$$$| $$| $$  \\ $$| $$  \\ $$  /$$$$$$$      | $$__  $$| $$  \\ $$| $$  \\ $$| $$$$$$/ \n");
	printf("| $$  \\ $$| $$  | $$ \\____  $$\\____  $$      | $$\\  $$$| $$| $$  | $$| $$  | $$ /$$__  $$      | $$  | $$| $$  | $$| $$  | $$| $$_  $$ \n");
	printf("| $$$$$$$/|  $$$$$$/ /$$$$$$$//$$$$$$$/      | $$ \\  $$| $$|  $$$$$$$|  $$$$$$$|  $$$$$$$      | $$  | $$|  $$$$$$/|  $$$$$$/| $$ \\  $$\n");
	printf("|_______/  \\______/ |_______/|_______/       |__/  \\__/|__/ \\____  $$ \\____  $$ \\_______/      |__/  |__/ \\______/  \\______/ |__/  \\__/\n");
	printf("                                                            /$$  \\ $$ /$$  \\ $$                                                        \n");
	printf("                                                           |  $$$$$$/|  $$$$$$/                                                        \n");
	printf("                                                            \\______/  \\______/                                                         \n");

	printf("\n");

	printf(" $> cheat base: 0x%p \n", bin);
#endif

	ctx.determine_version();

	ctx.m_settings.save(xors("default.crypt"));
	util::get_all_configs(ctx.m_cfg_list, xors("Software\\Spotify"));
	ctx.m_settings.load(xors("default.crypt"));

	while (!ctx.m_panic)
	{
		using namespace std::chrono_literals;

		auto mc = ctx.get_game();

		if (auto cur_hwnd = (HWND)mc->get_window_handle(); cur_hwnd != ctx.m_window)
		{
			ctx.m_window = cur_hwnd;
			hooked::o_wnd_proc = reinterpret_cast<decltype(&hooked::wnd_proc)>(SetWindowLongPtrA(ctx.m_window, GWLP_WNDPROC, (long long)hooked::wnd_proc));

			RECT rect;
			LI_FN(GetClientRect).cached()(ctx.m_window, &rect);

			ctx.m_screen_w = rect.right - rect.left;
			ctx.m_screen_h = rect.bottom - rect.top;
		}

		auto self = mc->get_player();
		auto world = mc->get_world();

		if (self && world)
		{
			ctx.m_ingame = true;

			HWND old_window = nullptr;
			if (ctx.m_menu_opening)
			{
				//mc->set_not_in_focus();
				mc->set_mouse_grabbed(false);
			}
			else if (ctx.m_menu_closing)
			{
				//mc->set_cursor_pos(mc->get_screen_w() / 2, mc->get_screen_h() / 2);
				//mc->set_in_focus();
				mc->set_mouse_grabbed(true);
			}

			ctx.m_in_chat = mc->is_in_chat();

			for (auto&& feature : ctx.m_features)
				feature->on_tick(mc, self, world);

			std::this_thread::sleep_for(1ms);
		}
		else
		{
			ctx.m_ingame = false;
			std::this_thread::sleep_for(1s);
		}
	}

	unload();

#ifdef TESTBUILD
	DWORD unload_time = GetTickCount() + 2000;
	while (true)
	{
		if (GetTickCount() > unload_time)
		{
			// need to call fclose on win10
			fclose(stdout);
			FreeConsole();

			// cant use FreeLibrary with manual map
			FreeLibraryAndExitThread(ctx.m_instance, 0);
		}

		Sleep(100);
	}
#else
#endif
}

void fuck_skids(void* bin)
{
	//DELETE_START(0);
	// stop most debuggers from working as breakpoint is patched to exit process call

	unsigned long old_protection = 0;

	uintptr_t exit_process = (uintptr_t)LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("kernel32.dll")), xors("ExitProcess"));
	uintptr_t dbg_ui_remote_breakin = (uintptr_t)LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("ntdll.dll")), xors("DbgUiRemoteBreakin"));
	uintptr_t dbg_break_point = (uintptr_t)LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("ntdll.dll")), xors("DbgBreakPoint"));

	// fuck DbgUiRemoteBreakin
	LI_FN(VirtualProtect).cached()((void*)dbg_ui_remote_breakin, 6, PAGE_EXECUTE_READWRITE, &old_protection);

	*(uint8_t*)(dbg_ui_remote_breakin) = 0x68; // push
	*(uintptr_t*)(dbg_ui_remote_breakin + 1) = exit_process;
	*(uint8_t*)(dbg_ui_remote_breakin + 5) = 0xC3; // ret

	LI_FN(VirtualProtect).cached()((void*)dbg_ui_remote_breakin, 6, old_protection, &old_protection);

	// fuck DbgBreakPoint
	LI_FN(VirtualProtect).cached()((void*)dbg_break_point, 6, PAGE_EXECUTE_READWRITE, &old_protection);

	*(uint8_t*)(dbg_break_point) = 0x68; // push
	*(uintptr_t*)(dbg_break_point + 1) = exit_process;
	*(uint8_t*)(dbg_break_point + 5) = 0xC3; // ret

	LI_FN(VirtualProtect).cached()((void*)dbg_break_point, 6, old_protection, &old_protection);

	LI_FN(VirtualProtect).cached()(bin, 4096, PAGE_READWRITE, &old_protection);
	RtlSecureZeroMemory(bin, 4096);

	//DELETE_END(0);
}

bool __stdcall DllMain(HINSTANCE instance, ulong32_t reason, void* reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
#ifdef TESTBUILD
		if (AllocConsole())
		{
			hwnd_t con_hwndw{GetConsoleWindow()};
			RECT con_bound{904 + 219, 420 + 420 + 69};
			RECT wndw_rect{};

			SetConsoleTitle(xors("mc"));

			GetWindowRect(con_hwndw, &wndw_rect);
			MoveWindow(con_hwndw, wndw_rect.left, wndw_rect.top, con_bound.left, con_bound.top, true);

			SetWindowLong(con_hwndw, GWL_STYLE, GetWindowLong(con_hwndw, GWL_STYLE) | WS_BORDER);
			SetWindowLong(con_hwndw, GWL_EXSTYLE, GetWindowLong(con_hwndw, GWL_EXSTYLE) | WS_EX_LAYERED);

			SetLayeredWindowAttributes(con_hwndw, 0, 230, 2);
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);

			freopen(xors("CONOUT$"), xors("w"), stdout);
		}
#else
		fuck_skids(instance);
		// take username and sub days left out of custom header
#endif

		LI_FN(CreateThread).cached()(0, 0, LPTHREAD_START_ROUTINE(hack), instance, 0, 0);

		return true;
	}

	return false;
}
