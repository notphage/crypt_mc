#include "context.h"
#include "header.h"
#include "MinHook.h"

#include <winternl.h>
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>

c_context ctx;

//int generate_dump(EXCEPTION_POINTERS* excp_ptr)
//{
//	// Open the file 
//	typedef BOOL(*PDUMPFN)(
//		HANDLE hProcess,
//		DWORD ProcessId,
//		HANDLE hFile,
//		MINIDUMP_TYPE DumpType,
//		PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
//		PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
//		PMINIDUMP_CALLBACK_INFORMATION CallbackParam
//		);
//	
//	BOOL bMiniDumpSuccessful;
//	WCHAR szPath[MAX_PATH];
//	WCHAR szFileName[MAX_PATH];
//	wchar_t szAppName[] = L"AppName";
//	wchar_t szVersion[] = L"v1.0";
//	DWORD dwBufferSize = MAX_PATH;
//	HANDLE hDumpFile;
//	SYSTEMTIME stLocalTime;
//	MINIDUMP_EXCEPTION_INFORMATION ExpParam;
//
//	GetLocalTime(&stLocalTime);
//	GetTempPathW(dwBufferSize, szPath);
//
//	wsprintfW(szFileName, L"%s%s", szPath, szAppName);
//	CreateDirectoryW(szFileName, NULL);
//
//	wsprintfW(szFileName, L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
//		szPath, szAppName, szVersion,
//		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
//		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond,
//		GetCurrentProcessId(), GetCurrentThreadId());
//	hDumpFile = CreateFileW(szFileName, GENERIC_READ | GENERIC_WRITE,
//		FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
//
//	ExpParam.ThreadId = GetCurrentThreadId();
//	ExpParam.ExceptionPointers = excp_ptr;
//	ExpParam.ClientPointers = TRUE;
//
//	HMODULE h = ::LoadLibraryW(L"DbgHelp.dll");
//	PDUMPFN pFn = (PDUMPFN)GetProcAddress(h, "MiniDumpWriteDump");
//
//	bMiniDumpSuccessful = (*pFn)(GetCurrentProcess(), GetCurrentProcessId(),
//		hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);
//
//	CloseHandle(hDumpFile);
//
//	return EXCEPTION_EXECUTE_HANDLER;
//}
//
//LONG WINAPI MyUnhandledExceptionFilter(
//	struct _EXCEPTION_POINTERS* ExceptionInfo
//)
//{
//	generate_dump(ExceptionInfo);
//	return EXCEPTION_EXECUTE_HANDLER;
//}

void hack(HINSTANCE bin)
{
	// store module base
	ctx.m_instance = bin;

	// parse header for user info and some interfaces
	parse_header(uintptr_t(bin));

	std::string cmd_str(LI_FN(GetCommandLineA).cached()());
	if (cmd_str.find(xors("forge")) != std::string::npos)
		ctx.m_client_flavor = FORGE;
	else if (cmd_str.find(xors("badlion")) != std::string::npos)
		ctx.m_client_flavor = BADLION;
	else if (cmd_str.find(xors("Hyperium")) != std::string::npos)
		ctx.m_client_flavor = HYPERIUM;
	else
		ctx.m_client_flavor = VANILLA;

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
	printf(" $> client flavor: %i\n", ctx.m_client_flavor.load());
	//SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);
#endif

	ctx.load();

	ctx.m_settings.save(xors("default.crypt"));
	util::get_all_configs(ctx.m_cfg_list, xors("Software\\Spotify"));
	ctx.m_settings.load(xors("default.crypt"));
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
	LI_FN(VirtualProtect).cached()(reinterpret_cast<void*>(dbg_ui_remote_breakin), 6, PAGE_EXECUTE_READWRITE, &old_protection);

	*reinterpret_cast<uint8_t*>(dbg_ui_remote_breakin) = 0x68; // push
	*reinterpret_cast<uintptr_t*>(dbg_ui_remote_breakin + 1) = exit_process;
	*reinterpret_cast<uint8_t*>(dbg_ui_remote_breakin + 5) = 0xC3; // ret

	LI_FN(VirtualProtect).cached()(reinterpret_cast<void*>(dbg_ui_remote_breakin), 6, old_protection, &old_protection);

	// fuck DbgBreakPoint
	LI_FN(VirtualProtect).cached()(reinterpret_cast<void*>(dbg_break_point), 6, PAGE_EXECUTE_READWRITE, &old_protection);

	*reinterpret_cast<uint8_t*>(dbg_break_point) = 0x68; // push
	*reinterpret_cast<uintptr_t*>(dbg_break_point + 1) = exit_process;
	*reinterpret_cast<uint8_t*>(dbg_break_point + 5) = 0xC3; // ret

	LI_FN(VirtualProtect).cached()(reinterpret_cast<void*>(dbg_break_point), 6, old_protection, &old_protection);

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

			freopen(xors("CONOUT$"), xors("w"), stderr);
			freopen(xors("CONOUT$"), xors("w"), stdout);
		}
#else 
		//fuck_skids(instance);
		// take username and sub days left out of custom header
#endif

		hack(instance);
	}

	return true;
}
