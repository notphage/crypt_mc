#include "context.h"
#include <windowsx.h>

decltype(hooked::o_swap_buffers) hooked::o_swap_buffers;

int __stdcall hooked::swap_buffers(HDC hdc)
{
	anti_rin(
		RECT rect;
		GetClientRect(ctx.m_window, &rect);

		ctx.m_screen_w = rect.right - rect.left;
		ctx.m_screen_h = rect.bottom - rect.top;

		ctx.m_renderer = std::make_shared<c_renderer>();
		ctx.m_renderer->m_render_list = ctx.m_renderer->make_render_list();
		ctx.m_renderer->m_font_context = ctx.m_renderer->make_font_context();
		ctx.m_renderer->m_fonts[font_normal] = ctx.m_renderer->create_font(xors("tahoma"), 12);
		ctx.m_gui = std::make_shared<c_gui>();

		ctx.m_init = true;
	)

	// proper frametime
	static auto old = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
	old = now;

	ctx.m_frametime = delta / 1000.f;

	//rendering shit goes here
	{
		ctx.m_gui->background();
		
		ctx.m_gui->draw();
		
		ctx.m_renderer->draw_scene();
	}

	return hooked::o_swap_buffers(hdc);
}
