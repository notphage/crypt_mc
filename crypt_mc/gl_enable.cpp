#include "context.h"
#include <gl/GL.h>
#include "glm/gtc/type_ptr.hpp"

decltype(hooked::o_gl_enable) hooked::o_gl_enable;

long __stdcall hooked::gl_enable(JNIEnv* jni, jclass caller, jint cap, jlong func_pointer)
{
	if (ctx.m_ingame && jni != nullptr && caller != nullptr && cap == GL_FOG)
	{
		std::shared_ptr<c_stack_trace> stack_class = std::make_shared<c_stack_trace>();
		stack_class->instantiate(jni);

		stack_trace stack;
		stack_class->get_stack_trace(5, stack);

		if (stack.valid)
		{
			if (stack.method_name == fnvc("func_78468_a") || stack.method_name == fnvc("a"))
			{
				const auto mc = ctx.get_game(jni);
				const auto self = mc->get_player();
				const auto world = mc->get_world();

				if (self && world)
				{
					glGetDoublev(GL_MODELVIEW_MATRIX, glm::value_ptr(ctx.m_view_matrix));
					glGetDoublev(GL_PROJECTION_MATRIX, glm::value_ptr(ctx.m_projection_matrix));

					// Don't ajust to actual camera height, game doesn't
					vec3 camera_origin(self->origin_x(), self->origin_y(), self->origin_z());
					vec3 prev_camera_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
					vec3 interp_camera_origin = prev_camera_origin + ((camera_origin - prev_camera_origin) * mc->get_render_partial_ticks());

					ctx.m_world_to_screen_matrix = ctx.m_projection_matrix * ctx.m_view_matrix;
					ctx.m_world_to_screen_matrix = glm::translate(ctx.m_world_to_screen_matrix, glm::f64vec3(-interp_camera_origin.x, -interp_camera_origin.y, -interp_camera_origin.z));
				}
			}
		}
	}

	return o_gl_enable(jni, caller, cap, func_pointer);
}