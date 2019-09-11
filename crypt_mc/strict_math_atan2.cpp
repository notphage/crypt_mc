#include "context.h"

decltype(hooked::o_strict_math_atan2) hooked::o_strict_math_atan2;

long __stdcall hooked::strict_math_atan2(JNIEnv* jni, jclass caller)
{
	if (ctx.m_ingame && jni != nullptr && caller != nullptr)
	{
		auto mc = ctx.get_game(jni);

		const auto self = mc->get_player();
		const auto world = mc->get_world();

		if (self && world)
		{
			for (auto&& feature : ctx.m_features)
				feature->run(mc, self, world, feature_type::FEATURE_ATAN2);
		}
	}

	return o_strict_math_atan2(jni, caller);
}