#include <windows.h>
#include "context.h"

decltype(hooked::o_wnd_proc) hooked::o_wnd_proc;

long long __stdcall hooked::wnd_proc(HWND hwnd, uint32_t msg, uint64_t uparam, int64_t param)
{
	if (LI_FN(GetActiveWindow).cached()() != hwnd)
	{
		if (g_input.is_any_input_active())
			g_input.clear();

		return CallWindowProcA(hooked::o_wnd_proc, hwnd, msg, uparam, param);
	}

	if (msg == WM_MOUSEMOVE)
		g_input.capture_mouse_move(param);

	if (msg == WM_SIZE)
	{
		RECT rect;
		LI_FN(GetClientRect).cached()(ctx.m_window, &rect);

		ctx.m_screen_w = rect.right - rect.left;
		ctx.m_screen_h = rect.bottom - rect.top;
	}

	if (msg == WM_DESTROY)
		ctx.m_panic = true;

	g_input.register_key_press(static_cast<VirtualKeyEvents_t>(msg), static_cast<VirtualKeys_t>(uparam));

	// dont pass mouse input to game when menu is open
	if (ctx.m_menu_open)
	{
		if (ctx.m_block_keyinput && (msg == VirtualKeyEvents_t::KEYDOWN || msg == VirtualKeyEvents_t::SYSKEYDOWN))
			return false;

		if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONUP
			|| msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDBLCLK || msg == WM_MOUSEFIRST || msg == WM_MOUSEWHEEL)
		{
			return false;
		}
	}

	return CallWindowProcA(hooked::o_wnd_proc, hwnd, msg, uparam, param);
}
