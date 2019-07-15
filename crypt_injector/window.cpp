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
	{
		ctx.m_panic = true;
		PostQuitMessage(0);

		return 0;
	}

	g_input.register_key_press(static_cast<VirtualKeyEvents_t>(msg), static_cast<VirtualKeys_t>(uparam));

	return DefWindowProcA(hwnd, msg, uparam, param);
}

void c_window::create()
{
	// Register the window class.
	const char CLASS_NAME[] = "Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = &wnd_proc;
	wc.hInstance = ctx.m_instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		ctx.m_instance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == nullptr)
		return;

	ctx.m_window = hwnd;

	ShowWindow(hwnd, SW_SHOW);

	m_renderer->start();
}

void c_window::render()
{
	m_renderer->begin();

	// drawing goes here

	m_renderer->end();
}

void c_window::end()
{
	m_renderer->release();
}
