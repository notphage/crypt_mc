#include "context.h"

long long __stdcall c_window::wnd_proc(HWND hwnd, uint32_t msg, uint64_t uparam, int64_t param)
{
	if (msg == WM_DESTROY)
	{
		ctx.m_panic = true;
		LI_FN(PostQuitMessage).cached()(0);

		return 0; 
	}

	if (GetActiveWindow() != hwnd)
	{
		if (g_input.is_any_input_active())
			g_input.clear();

		return DefWindowProcA(hwnd, msg, uparam, param);
	}

	if (msg == WM_MOUSEMOVE)
		g_input.capture_mouse_move(param);

	g_input.register_key_press(static_cast<VirtualKeyEvents_t>(msg), static_cast<VirtualKeys_t>(uparam));

	return DefWindowProcA(hwnd, msg, uparam, param);
}

void c_window::create()
{
	// Register the window class.
	const char CLASS_NAME[] = " ";

	WNDCLASS wc = { };

	wc.lpfnWndProc = &wnd_proc;
	wc.hInstance = ctx.m_instance;
	wc.lpszClassName = CLASS_NAME;

	LI_FN(RegisterClassA).cached()(&wc);

	RECT desk_rect{};

	// Get a handle to the desktop window
	const HWND desk = LI_FN(GetDesktopWindow).cached()();

	// Get the size of screen to the variable desktop
	LI_FN(GetWindowRect).cached()(desk, &desk_rect);

	HWND hwnd = LI_FN(CreateWindowExA).cached()(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"",    // Window text
		WS_BORDER | WS_EX_TOPMOST,            // Window style

		// Size and position
		(desk_rect.right / 2) - 160, (desk_rect.bottom / 2) - 120, ctx.m_screen_w, ctx.m_screen_h,

		NULL,       // Parent window    
		NULL,       // Menu
		ctx.m_instance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == nullptr)
		return;

	ctx.m_window = hwnd;

	LI_FN(SetWindowLongA).cached()(hwnd, GWL_STYLE, 0);  // Without 1 point border = white rectangle 
	LI_FN(SetWindowPos).cached()(hwnd, HWND_TOPMOST, (desk_rect.right / 2) - 160, (desk_rect.bottom / 2) - 120, ctx.m_screen_w, ctx.m_screen_h, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
	LI_FN(ShowWindow).cached()(hwnd, SW_SHOW);

	ctx.m_renderer = std::make_unique<c_renderer>(8192);

	ctx.m_renderer->start();
}

void c_window::render()
{
	ctx.m_renderer->begin();

	m_gui.draw();
	//ctx.m_renderer->draw_filled_rect({ 0, 0, 200, 200 }, color_t(255, 255, 255, 100));

	ctx.m_renderer->render();
	ctx.m_renderer->end();
}

void c_window::end()
{
	ctx.m_renderer->release();
}
