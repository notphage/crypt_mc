#include "context.h"
#include <windowsx.h>

decltype(hooked::o_swap_buffers) hooked::o_swap_buffers;

static HGLRC crypt_ctx = nullptr;
long __stdcall hooked::swap_buffers(JNIEnv* jni, jclass caller, jobject a3)
{
	void* buf = jni->GetDirectBufferAddress(a3);
	HDC hdc = *(HDC*)((uintptr_t)buf + 8);

	HGLRC old_ctx = wglGetCurrentContext();
	if (!ctx.m_init)
	{
		crypt_ctx = wglCreateContext(hdc);
		wglMakeCurrent(hdc, crypt_ctx);
		
		RECT rect;
		LI_FN(GetClientRect).cached()(ctx.m_window, &rect);

		ctx.m_screen_w = rect.right - rect.left;
		ctx.m_screen_h = rect.bottom - rect.top;

		ctx.m_renderer = std::make_shared<c_renderer>();
		ctx.m_renderer->m_render_list = ctx.m_renderer->make_render_list();
		ctx.m_renderer->m_font_context = ctx.m_renderer->make_font_context();
		ctx.m_renderer->m_fonts[font_normal] = ctx.m_renderer->create_font(xors("tahoma"), 12);
		ctx.m_gui = std::make_shared<c_gui>();

		ctx.m_init = true;
	}
	else
		wglMakeCurrent(hdc, crypt_ctx);

	// proper frametime
	static auto old = std::chrono::high_resolution_clock::now();
	const auto now = std::chrono::high_resolution_clock::now();
	const float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
	old = now;

	ctx.m_frametime = delta / 1000.f;

	//rendering shit goes here
	ctx.m_renderer->draw_begin();
	{
		ctx.m_gui->background();
		ctx.m_gui->draw();
		ctx.m_renderer->draw_scene();
	}
	ctx.m_renderer->draw_end();

	wglMakeCurrent(hdc, old_ctx);

	return o_swap_buffers(jni, caller, a3);
}
