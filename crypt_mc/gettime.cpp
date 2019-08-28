#include "context.h"
#include <gl/GL.h>

decltype(hooked::o_get_time) hooked::o_get_time;

long __stdcall hooked::get_time(JNIEnv* jni, jclass caller)
{
	if (ctx.m_ingame && jni != nullptr && caller != nullptr)
	{
		std::shared_ptr<c_stack_trace> stack_class = std::make_shared<c_stack_trace>();
		stack_class->instantiate(jni);

		stack_trace stack;
		stack_class->get_stack_trace(4, stack);

		if (stack.valid)
		{
			auto mc = ctx.get_game(jni);

			const auto self = mc->get_player();
			const auto world = mc->get_world();
			
			if (self && world)
			{
				if (stack.method_name == fnvc("func_71407_l")) // func_71407_l
				{
					for (auto&& feature : ctx.m_features)
						feature->on_time(mc, self, world);
				}
			}
		}
	}

	return o_get_time(jni, caller);
}