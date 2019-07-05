#include "context.h"

long long __stdcall c_window::wnd_proc(HWND hwnd, uint32_t msg, uint64_t uparam, int64_t param)
{
	if (GetActiveWindow() != hwnd)
	{
		if (g_input.is_any_input_active())
			g_input.clear();

		return DefWindowProcA(hwnd, msg, uparam, param);
	}

	if (msg == WM_MOUSEMOVE)
		g_input.capture_mouse_move(param);

	if (msg == WM_DESTROY)
		ctx.m_panic = true;

	g_input.register_key_press(static_cast<VirtualKeyEvents_t>(msg), static_cast<VirtualKeys_t>(uparam));

	return DefWindowProcA(hwnd, msg, uparam, param);
}

void c_window::create()
{
	const char win_name[] = "wclass";
	WNDCLASS wc = { };

	wc.lpfnWndProc = &wnd_proc;
	wc.hInstance = ctx.m_instance;
	wc.lpszClassName = win_name;

	RegisterClass(&wc);

	//HWND hwnd = CreateWindowExA(0, nullptr, nullpyt, WS_BORDER, 0, 0, 200, 200, nullptr, nullptr, ctx.m_instance, 0);
	HWND hwnd = CreateWindowExA(0, win_name, nullptr, WS_BORDER, CW_USEDEFAULT, CW_USEDEFAULT, 256, 172, nullptr, nullptr, ctx.m_instance, 0);

	if (hwnd == nullptr)
		return;

	ctx.m_window = hwnd;

	SetWindowLong(hwnd, GWL_STYLE, 0);  // Without 1 point border = white rectangle 
	SetWindowPos(hwnd, 0, 150, 100, 250, 250, SWP_FRAMECHANGED);
	ShowWindow(hwnd, SW_SHOW);
}
