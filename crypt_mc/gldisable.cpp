#include "context.h"
#include <gl/GL.h>

decltype(hooked::o_gldisable) hooked::o_gldisable;

void __stdcall hooked::gldisable(JNIEnv* jni, jclass caller, jint cap, jlong func_pointer)
{
    hooked::o_gldisable(jni, caller, cap, func_pointer);

	//if (cap == GL_FOG && ctx.m_ingame)
	//{
	//	std::shared_ptr<c_stack_trace> stack_class = std::make_shared<c_stack_trace>();
	//	stack_class->instantiate(jni);
	//
	//	stack_trace stack;
	//	stack_class->get_stack_trace(2, stack);
	//
    //    if (stack.valid) 
    //    {
	//		if (std::string(stack.method_name) == std::string(xors("func_78471_a"))) //func_78471_a
    //        {
	//			auto mc = ctx.get_game(jni);
	//
	//			auto self = mc->get_player();
	//			auto world = mc->get_world();
	//
	//			if (self && world)
	//			{
    //                ctx.m_renderer.game_begin();
    //                {
    //                    for (auto&& feature : ctx.m_features)
    //                        feature->on_render(mc, self, world);
    //                }
    //                ctx.m_renderer.game_end();
    //            }
    //        }
    //    }
	//}
}