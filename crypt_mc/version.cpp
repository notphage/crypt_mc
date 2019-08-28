#include "context.h"
#include "MinHook.h"

// SDK include
#include "sdk_1.7.10.h"
#include "sdk_forge_1.7.10.h"

#include "sdk_1.8.X.h"
#include "sdk_forge_1.8.X.h"

// Feature include
#include "fast_place.h"
#include "auto_clicker.h"
#include "aim_assist.h"
#include "timer.h"
#include "velocity.h"
#include "speed.h"
#include "throw.h"
#include "visuals.h"
#include "reach.h"
#include "flight.h"
#include "sprint.h"
#include "step.h"
#include "anti_afk.h"

int __stdcall enum_windows_proc(HWND hwnd, int64_t lparam)
{
	const DWORD TITLE_SIZE = 1024;
	char window_title[TITLE_SIZE];

	GetWindowTextA(hwnd, window_title, TITLE_SIZE);

	std::string title(window_title);

	if (title.find(xors("1.7.10")) != std::string::npos)
	{
#ifdef TESTBUILD
		printf(xors(" $> Version 1.7.10 loaded\n"));
#endif

		ctx.m_window = hwnd;
		ctx.m_version = MC_1710;

		return 0;
	}
	
	if (title.find(xors("1.8")) != std::string::npos || title.find(xors("1.8.8")) != std::string::npos || title.find(xors("1.8.9")) != std::string::npos)
	{
#ifdef TESTBUILD
		printf(xors(" $> Version 1.8.X loaded\n"));
#endif

		ctx.m_window = hwnd;
		ctx.m_version = MC_18X;

		return 0;
	}

#ifdef TESTBUILD
	printf(xors(" $> Version failed to be determined\n"));
#endif

	ctx.m_window = nullptr;
	ctx.m_version = MC_UNKNOWN;

	return 1;
}

void c_context::determine_version()
{
	ctx.m_renderer = std::make_unique<c_renderer>();

	// begin hooking
	{
        typedef jint(__stdcall *get_created_java_vms_proc)(JavaVM**, jsize, jsize*);

        get_created_java_vms_proc fn_get_created_java_vms = (get_created_java_vms_proc)LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("jvm.dll")), xors("JNI_GetCreatedJavaVMs"));
        fn_get_created_java_vms(&m_jvm, 1, nullptr);

        jint err = m_jvm->AttachCurrentThread(reinterpret_cast<void**>(&m_jni), nullptr);
        if (err != JNI_OK)
            m_jni = nullptr;

		ctx.m_forge = m_jni->FindClass(xors("net/minecraftforge/classloading/FMLForgePlugin")) != nullptr;

		EnumWindows(enum_windows_proc, 0);

#ifdef TESTBUILD
		printf(xors(" $> window base: 0x%p\n"), ctx.m_window);
#endif

		MH_Initialize();

		// Swap Buffers
		MH_CreateHook(SwapBuffers, &hooked::swap_buffers, reinterpret_cast<void**>(&hooked::o_swap_buffers));
		MH_EnableHook(SwapBuffers);

		auto gl_disable = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_GL11_nglDisable"));

		// glDisable
		MH_CreateHook(gl_disable, &hooked::gldisable, reinterpret_cast<void**>(&hooked::o_gldisable));
		MH_EnableHook(gl_disable);

		auto get_time = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_WindowsSysImplementation_nGetTime"));

		// getTime
		MH_CreateHook(get_time, &hooked::get_time, reinterpret_cast<void**>(&hooked::o_get_time));
		MH_EnableHook(get_time);

		hooked::o_wnd_proc = reinterpret_cast<decltype(&hooked::wnd_proc)>(SetWindowLongPtrA(ctx.m_window, -4, (long long)hooked::wnd_proc));
	}

	ctx.m_features.push_back(std::make_unique<c_auto_clicker>());
	ctx.m_features.push_back(std::make_unique<c_aim_assist>());
	ctx.m_features.push_back(std::make_unique<c_timer>());
	ctx.m_features.push_back(std::make_unique<c_velocity>());
	ctx.m_features.push_back(std::make_unique<c_fast_place>());
	ctx.m_features.push_back(std::make_unique<c_speed>());
	ctx.m_features.push_back(std::make_unique<c_throw>());
	ctx.m_features.push_back(std::make_unique<c_visuals>());
	ctx.m_features.push_back(std::make_unique<c_reach>());
    ctx.m_features.push_back(std::make_unique<c_flight>());
    ctx.m_features.push_back(std::make_unique<c_sprint>());
    ctx.m_features.push_back(std::make_unique<c_step>());
	ctx.m_features.push_back(std::make_unique<c_anti_afk>());
}

jvmtiEnv* c_context::get_jvmti_env()
{
	if (!ctx.m_jvm)
		return nullptr;

	jvmtiEnv* jvmti;
	ctx.m_jvm->GetEnv(reinterpret_cast<void**>(&jvmti), JVMTI_VERSION_1_2);

	return jvmti;
}

std::shared_ptr<c_class_loader> c_context::get_class_loader(JNIEnv* _jni)
{
	std::shared_ptr<c_class_loader> ptr;

	switch (m_version)
	{
		case MC_1710:
		{
			if (ctx.m_forge)
				ptr = std::make_shared<c_class_loader_forge_1710>();
			else
				ptr = std::make_shared<c_class_loader_1710>();
			
			break;
		}

		case MC_18X:
		{
			if (ctx.m_forge)
				ptr = std::make_shared<c_class_loader_forge_18X>();
			else
				ptr = std::make_shared<c_class_loader_18X>();
			
			break;
		}

		case MC_UNKNOWN:
		default:
		{
			return nullptr;
		}
	}

	ptr->instantiate(_jni ? _jni : ctx.m_jni);

	return ptr;
}

std::shared_ptr<c_game> c_context::get_game(JNIEnv* _jni)
{
	std::shared_ptr<c_game> ptr;

	switch (m_version)
	{
		case MC_1710:
		{
			if (ctx.m_forge)
				ptr = std::make_shared<c_game_forge_1710>();
			else
				ptr = std::make_shared<c_game_1710>();
			
			break;
		}

		case MC_18X:
		{
			if (ctx.m_forge)
				ptr = std::make_shared<c_game_forge_18X>();
			else
				ptr = std::make_shared<c_game_18X>();
			
			break;
		}

		case MC_UNKNOWN:
		default:
		{
			return nullptr;
		}
	}

	ptr->instantiate(_jni ? _jni : ctx.m_jni);

	return ptr;
}

