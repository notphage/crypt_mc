#include "context.h"
#include <Windows.h>
#include <iostream>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

c_context ctx;

void setup_console()
{
	SetConsoleTitleW(xors(L"Rök Injector"));
	system(xors("Color 0E"));
	printf(R"( ____            __      ______                               __                   
/\  _`\         /\ \    /\__  _\            __               /\ \__                
\ \ \L\ \    ___\ \ \/'\\/_/\ \/     ___   /\_\     __    ___\ \ ,_\   ___   _ __  
 \ \ ,  /   / __`\ \ , <   \ \ \   /' _ `\ \/\ \  /'__`\ /'___\ \ \/  / __`\/\`'__\
  \ \ \\ \ /\ \L\ \ \ \\`\  \_\ \__/\ \/\ \ \ \ \/\  __//\ \__/\ \ \_/\ \L\ \ \ \/ 
   \ \_\ \_\ \____/\ \_\ \_\/\_____\ \_\ \_\_\ \ \ \____\ \____\\ \__\ \____/\ \_\ 
    \/_/\/ /\/___/  \/_/\/_/\/_____/\/_/\/_/\ \_\ \/____/\/____/ \/__/\/___/  \/_/ 
                                           \ \____/                                
                                            \/___/                                 

__________________________________________________________________________________
)");
}

std::string make_full_path(const std::string& name)
{
	char tmp_path[MAX_PATH];
	GetModuleFileName(GetModuleHandleA(nullptr), tmp_path, (sizeof(tmp_path)));

	PathRemoveFileSpec(tmp_path);
	std::string path(tmp_path);
	path += std::string("\\") + name;
	return path;
}

void Inject(int pid, const std::string& path, bool can_manual_map)
{
	bool loadlibrayyyyyyyyyyyyy = true;
	bool did_inject = false;

	if (can_manual_map)
	{
		printf("> Default injection method: Manual Map\n");
		printf("> Do you want to switch to LoadLibrary?\n");
		printf("> Press enter to continue...");

		std::string input;
		getline(std::cin, input);
		loadlibrayyyyyyyyyyyyy = !input.empty();
	}

	if (loadlibrayyyyyyyyyyyyy)
	{
		did_inject = false;
		printf("> LoadLibrary not supported\n");
	}
	else
		did_inject = ctx.m_injector.inject_from_path(pid, path.c_str());

	if (!did_inject)
		printf("> Injection failed\n");
}

void close() {
	printf("> Press enter to close\n");
	printf("> Automatically closing in 5 seconds\n");

	Sleep(500);
	DWORD finished_inject = GetTickCount64();
	while (!(GetAsyncKeyState(VK_RETURN) < 0)) {
		DWORD current = GetTickCount64();
		if (current > finished_inject + 5000)
			return;

		Sleep(100);
	}
}

int32_t __stdcall WinMain(HINSTANCE instance, HINSTANCE prev_instance, char* cmd_line, int32_t cmd_show)
{
	ctx.m_instance = instance;
	//setup_console();
	ctx.m_loader_window.create();

	MSG msg;

	while (PeekMessageA(&msg, ctx.m_window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessageA(&msg);

		if (msg.message == WM_QUIT)
			break;

		ctx.m_loader_window.render();
	}

	ctx.m_loader_window.end();

	//target_process target;
	//int pid = c_target::find_target(&target);
	//if (pid != -1)
	//{
	//	printf("> Target: %s \n", target.display_name.c_str());
	//	printf("> Process: %s \n", target.process_name.c_str());
	//	printf("> Process ID: %i \n", pid);
	//	printf("> DLL: %s \n", target.dll_name.c_str());
	//
	//	std::string path = make_full_path(target.dll_name);
	//	printf("> DLL Path: %s \n", path.c_str());
	//
	//	Inject(pid, path, target.manual_map);
	//}
	//else
	//{
	//	printf("> Couldn't find any valid target...\n");
	//}

	c_client client;
	client.run();

	getchar();

	close();
	return 0;
}