#include "context.h"
#include <windowsx.h>
#include <gl\GL.h>
#include "glm/gtc/type_ptr.hpp"

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

	auto mc = ctx.get_game(jni);
	const auto self = mc->get_player();
	const auto world = mc->get_world();

	// Get the world to screen matrix and compensate for negative camera origin
	if (self && world)
	{
		glPushMatrix();
		glLoadIdentity();

		mc->setup_camera_transform(mc->get_render_partial_ticks(), 0);

		glGetDoublev(GL_MODELVIEW_MATRIX, glm::value_ptr(ctx.m_view_matrix));
		glGetDoublev(GL_PROJECTION_MATRIX, glm::value_ptr(ctx.m_projection_matrix));

		glPopMatrix();

		// Don't ajust to actual camera height, game doesn't
		vec3 camera_origin(self->origin_x(), self->origin_y(), self->origin_z());
		vec3 prev_camera_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
		vec3 interp_camera_origin = prev_camera_origin + ((camera_origin - prev_camera_origin) * mc->get_render_partial_ticks());

		ctx.m_world_to_screen_matrix = ctx.m_projection_matrix * ctx.m_view_matrix;
		ctx.m_world_to_screen_matrix = glm::translate(ctx.m_world_to_screen_matrix, glm::f64vec3(-interp_camera_origin.x, -interp_camera_origin.y, -interp_camera_origin.z));
	}

	//rendering shit goes here
	ctx.m_renderer->draw_begin();
	{
		if (self && world)
			for (auto&& feature : ctx.m_features)
				feature->run(jni, mc, self, world, feature_type::FEATURE_SWAP_BUFFERS);

		ctx.m_gui->background();
		ctx.m_gui->draw();

		ctx.m_renderer->draw_scene();
	}
	ctx.m_renderer->draw_end();

	wglMakeCurrent(hdc, old_ctx);

	return o_swap_buffers(jni, caller, a3);
}
