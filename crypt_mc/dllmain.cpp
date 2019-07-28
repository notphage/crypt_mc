#include "context.h"
#include <VersionHelpers.h>
#include "erase.h"
#include "header.h"
#include "MinHook.h"

c_context ctx;

// to get rid of a string in microsoft crt
static void __cdecl std::_Xlength_error(const char* error) {}

void unload()
{
	auto gl_disable = GetProcAddress(GetModuleHandleA(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_GL11_nglDisable"));
    auto get_time = GetProcAddress(GetModuleHandle(xors("lwjgl64.dll")), xors("Java_org_lwjgl_WindowsSysImplementation_nGetTime")); 

	MH_RemoveHook(SwapBuffers);
	MH_RemoveHook(gl_disable);
    MH_RemoveHook(get_time);

	SetWindowLongPtrA(ctx.m_window, -4, (long long)hooked::o_wnd_proc);

	MH_Uninitialize();
}

void hack(HINSTANCE bin)
{
	//DELETE_START(0);

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

	// anti segfault
	// nothing but trouble
	util::create_file(xors("./default.cfg"), OPEN_EXISTING);
	ctx.m_cfg_list.push_back(xors("default"));
	util::get_all_files(ctx.m_cfg_list, xors("./"));
	//ctx.m_settings.load(xors("./default.cfg"));

	while (!ctx.m_panic)
	{
		using namespace std::chrono_literals;

		auto mc = ctx.get_game();

		auto self = mc->get_player();
		auto world = mc->get_world();

		if (self && world)
		{
			ctx.m_ingame = true;

			if (ctx.m_menu_opening)
			{
				mc->set_not_in_focus();
			}
			else if (ctx.m_menu_closing)
			{
				mc->set_in_focus();
			}

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

	//DELETE_END(0);

	unload();

//#ifdef TESTBUILD
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
//#else
//	// im pro choice so its okay
//	abort();
//#endif
}

void fuck_skids()
{
	//DELETE_START(0);
	// stop most debuggers from working as breakpoint is patched to exit process call

	unsigned long old_protection = 0;

	uintptr_t exit_process = (uintptr_t)GetProcAddress(GetModuleHandleA(xors("kernel32.dll")), xors("ExitProcess"));
	uintptr_t dbg_ui_remote_breakin = (uintptr_t)GetProcAddress(GetModuleHandleA(xors("ntdll.dll")), xors("DbgUiRemoteBreakin"));
	uintptr_t dbg_break_point = (uintptr_t)GetProcAddress(GetModuleHandleA(xors("ntdll.dll")), xors("DbgBreakPoint"));

	// fuck DbgUiRemoteBreakin
	VirtualProtect((void*)dbg_ui_remote_breakin, 6, PAGE_EXECUTE_READWRITE, &old_protection);

	*(uint8_t*)(dbg_ui_remote_breakin) = 0x68; // push
	*(uintptr_t*)(dbg_ui_remote_breakin + 1) = exit_process;
	*(uint8_t*)(dbg_ui_remote_breakin + 5) = 0xC3; // ret

	VirtualProtect((void*)dbg_ui_remote_breakin, 6, old_protection, &old_protection);

	// fuck DbgBreakPoint
	VirtualProtect((void*)dbg_break_point, 6, PAGE_EXECUTE_READWRITE, &old_protection);

	*(uint8_t*)(dbg_break_point) = 0x68; // push
	*(uintptr_t*)(dbg_break_point + 1) = exit_process;
	*(uint8_t*)(dbg_break_point + 5) = 0xC3; // ret

	VirtualProtect((void*)dbg_break_point, 6, old_protection, &old_protection);

	// break tools like scylla and such as they check signatures on dos/nt header

	// we dont have a header in public mode

	/*
	// fuck dos header signature
	*(uint16_t*)( process ) = 0;

	// fuck nt header signature
	*(uint32_t*)( process + ( (IMAGE_DOS_HEADER*)process )->e_lfanew ) = 0;
	*/

	//DELETE_END(0);
}

bool __stdcall DllMain(HINSTANCE instance, ulong32_t reason, void* reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		//DisableThreadLibraryCalls(instance);

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
        if (AllocConsole())
        {
            hwnd_t con_hwndw{ GetConsoleWindow() };
            RECT con_bound{ 904 + 219, 420 + 420 + 69 };
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
		//fuck_skids();
		// take username and sub days left out of custom header
#endif

		CreateThread(0, 0, LPTHREAD_START_ROUTINE(hack), instance, 0, 0);

		return true;
	}

	return false;
}
