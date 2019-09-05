#include "context.h"

decltype(hooked::o_get_update) hooked::o_get_update;

long __stdcall hooked::get_update(JNIEnv* jni, jclass caller)
{	
	if (jni != nullptr && caller != nullptr && ctx.m_init)
	{
		std::shared_ptr<c_stack_trace> stack_class = std::make_shared<c_stack_trace>();
		stack_class->instantiate(jni);
	
		stack_trace stack;
		stack_class->get_stack_trace(6, stack);
	
		if (stack.valid)
		{
			auto mc = ctx.get_game(jni);
	
			const auto self = mc->get_player();
			const auto world = mc->get_world();

			if (self.get() != nullptr && world.get() != nullptr)
			{
				ctx.m_ingame = true;

				if (stack.method_name == fnvc("func_71411_J") ||
					stack.method_name == fnvc("ak") ||
					stack.method_name == fnvc("av")) // func_71411_J
				{
					if (ctx.m_menu_opening)
					{
						//mc->set_not_in_focus();
						mc->set_mouse_grabbed(false);
					}
					else if (ctx.m_menu_closing)
					{
						//mc->set_cursor_pos(mc->get_screen_w() / 2, mc->get_screen_h() / 2);
						//mc->set_in_focus();
						mc->set_mouse_grabbed(true);
					}
	
					ctx.m_in_chat = mc->is_in_chat();
	
					for (auto&& feature : ctx.m_features)
						feature->on_tick(mc, self, world);
				}
			}
			else
				ctx.m_ingame = false;
		}
	}

	return o_get_update(jni, caller);
}