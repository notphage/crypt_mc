#include "context.h"

decltype(hooked::o_get_update) hooked::o_get_update;

long __stdcall hooked::get_update(JNIEnv* jni, jclass caller)
{	
	//if (ctx.m_panic)
	//	ctx.m_client.set_shared_mem_stage(shared_mem_stage::STAGE_CLOSE);

	if (jni != nullptr && caller != nullptr && ctx.m_init)
	{
		std::shared_ptr<c_stack_trace> stack_class = std::make_shared<c_stack_trace>();
		stack_class->instantiate(jni);
	
		stack_trace stack;
		stack_class->get_stack_trace(6, stack);
	
		if (stack.valid)
		{
			auto mc = ctx.get_game(jni);

			if (const auto cur_hwnd = reinterpret_cast<HWND>(mc->get_window_handle()); cur_hwnd != ctx.m_window)
			{
				ctx.m_window = cur_hwnd;
				hooked::o_wnd_proc = reinterpret_cast<decltype(&hooked::wnd_proc)>(SetWindowLongPtrA(ctx.m_window, GWLP_WNDPROC, reinterpret_cast<long long>(hooked::wnd_proc)));

				RECT rect;
				LI_FN(GetClientRect).cached()(ctx.m_window, &rect);

				ctx.m_screen_w = rect.right - rect.left;
				ctx.m_screen_h = rect.bottom - rect.top;
			}
	
			const auto self = mc->get_player();
			const auto world = mc->get_world();
			
			if (self && world)
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
						feature->run(mc, self, world, feature_type::FEATURE_UPDATE);
				}
			}
			else
				ctx.m_ingame = false;
		}
	}

	return o_get_update(jni, caller);
}