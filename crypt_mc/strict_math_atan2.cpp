#include "context.h"

decltype(hooked::o_strict_math_atan2) hooked::o_strict_math_atan2;

long __stdcall hooked::strict_math_atan2(JNIEnv* jni, jclass caller)
{
	if (ctx.m_ingame && jni != nullptr && caller != nullptr)
	{
		std::shared_ptr<c_stack_trace> stack_class = std::make_shared<c_stack_trace>();
		stack_class->instantiate(jni);

		stack_trace stack;
		stack_class->get_stack_trace(4, stack);

		if (stack.valid)
		{
			if (stack.method_name == fnvc("func_94248_c") || stack.method_name == fnvc("d"))
			{
				auto mc = ctx.get_game(jni);

				const auto self = mc->get_player();
				const auto world = mc->get_world();

				if (self && world)
				{
					for (auto&& feature : ctx.m_features)
						feature->run(jni, mc, self, world, feature_type::FEATURE_ATAN2);
				}
			}
		}
	}

	return o_strict_math_atan2(jni, caller);
}