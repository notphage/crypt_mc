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
#include "fast_stop.h"
#include "safe_walk.h"
#include "fullbright.h"

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
	
	if (title.find(xors("1.8")) != std::string::npos || title.find(xors("1.8.8")) != std::string::npos || title.find(xors("1.8.9")) != std::string::npos || title.find(xors("Hyperium")) != std::string::npos)
	{
#ifdef TESTBUILD
		printf(xors(" $> Version 1.8.X loaded\n"));
#endif

		ctx.m_window = hwnd;
		ctx.m_version = MC_18X;

		return 0;
	}

	ctx.m_window = nullptr;
	ctx.m_version = MC_UNKNOWN;

	for (int i = 0; i < sizeof(window_title); i++) {
		window_title[i] = '\0';
	}

	return 1;
}

void c_context::load()
{
	ctx.m_renderer = std::make_unique<c_renderer>();

	// begin hooking
	{
		EnumWindows(enum_windows_proc, 0);

#ifdef TESTBUILD
		printf(xors(" $> window base: 0x%p\n"), ctx.m_window);
#endif

		MH_Initialize();

		auto swap_buffers = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_WindowsContextImplementation_nSwapBuffers"));
		MH_CreateHook(swap_buffers, &hooked::swap_buffers, reinterpret_cast<void**>(&hooked::o_swap_buffers));
		MH_EnableHook(swap_buffers);

		auto update = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_WindowsDisplay_nUpdate"));
		MH_CreateHook(update, &hooked::get_update, reinterpret_cast<void**>(&hooked::o_get_update));
		MH_EnableHook(update);
		
		auto get_time = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_WindowsSysImplementation_nGetTime"));
		MH_CreateHook(get_time, &hooked::get_time, reinterpret_cast<void**>(&hooked::o_get_time));
		MH_EnableHook(get_time);

		auto strict_math_atan2 = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("java.dll")), xors("Java_java_lang_StrictMath_atan2"));
		MH_CreateHook(strict_math_atan2, &hooked::strict_math_atan2, reinterpret_cast<void**>(&hooked::o_strict_math_atan2));
		MH_EnableHook(strict_math_atan2);

		hooked::o_wnd_proc = reinterpret_cast<decltype(&hooked::wnd_proc)>(SetWindowLongPtrA(ctx.m_window, -4, (long long)hooked::wnd_proc));
	}

	ctx.m_features.emplace_back(std::make_unique<c_aim_assist>(&ctx.m_settings.combat_aim_assist, &ctx.m_settings.combat_aim_assist_key));
	ctx.m_features.emplace_back(std::make_unique<c_anti_afk>(&ctx.m_settings.player_anti_afk));
	ctx.m_features.emplace_back(std::make_unique<c_auto_clicker>(&ctx.m_settings.combat_auto_clicker, &ctx.m_settings.combat_auto_clicker_key));
	ctx.m_features.emplace_back(std::make_unique<c_fast_place>(&ctx.m_settings.player_fast_place, &ctx.m_settings.player_fast_place_key));
	ctx.m_features.emplace_back(std::make_unique<c_fullbright>(&ctx.m_settings.visuals_fullbright, &ctx.m_settings.visuals_fullbright_key));
	ctx.m_features.emplace_back(std::make_unique<c_fast_stop>(&ctx.m_settings.movement_fast_stop, &ctx.m_settings.movement_fast_stop_key));
	ctx.m_features.emplace_back(std::make_unique<c_flight>(&ctx.m_settings.movement_flight, &ctx.m_settings.movement_flight_key));
	ctx.m_features.emplace_back(std::make_unique<c_reach>(&ctx.m_settings.combat_reach, &ctx.m_settings.combat_reach_key));
	ctx.m_features.emplace_back(std::make_unique<c_safe_walk>(&ctx.m_settings.movement_safe_walk, &ctx.m_settings.movement_safe_walk_key));
	ctx.m_features.emplace_back(std::make_unique<c_speed>(&ctx.m_settings.movement_speed, &ctx.m_settings.movement_speed_key));
	ctx.m_features.emplace_back(std::make_unique<c_long_jump>(&ctx.m_settings.movement_speed, &ctx.m_settings.movement_longjump_key));
	ctx.m_features.emplace_back(std::make_unique<c_sprint>(&ctx.m_settings.movement_sprint, &ctx.m_settings.movement_sprint_key));
	ctx.m_features.emplace_back(std::make_unique<c_air_control>(&ctx.m_settings.movement_air_control, &ctx.m_settings.movement_air_control_key));
	ctx.m_features.emplace_back(std::make_unique<c_step>(&ctx.m_settings.movement_step, &ctx.m_settings.movement_step_key));
	//ctx.m_features.emplace_back(std::make_unique<c_throw>(&ctx.m_settings.player_throw));
	ctx.m_features.emplace_back(std::make_unique<c_positive_timer>(&ctx.m_settings.misc_timer, &ctx.m_settings.misc_positive_timer_key));
	ctx.m_features.emplace_back(std::make_unique<c_negative_timer>(&ctx.m_settings.misc_timer, &ctx.m_settings.misc_negative_timer_key));
	ctx.m_features.emplace_back(std::make_unique<c_velocity>(&ctx.m_settings.combat_velocity, &ctx.m_settings.combat_velocity_key));
}

void c_context::unload()
{
	auto swap_buffers = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_WindowsContextImplementation_nSwapBuffers"));
	auto get_time = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_WindowsSysImplementation_nGetTime"));
	auto update = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("lwjgl64.dll")), xors("Java_org_lwjgl_opengl_WindowsDisplay_nUpdate"));
	auto strict_math_atan2 = LI_FN(GetProcAddress).cached()(LI_FN(GetModuleHandleA).cached()(xors("java.dll")), xors("Java_java_lang_StrictMath_atan2"));

	MH_RemoveHook(swap_buffers);
	MH_RemoveHook(get_time);
	MH_RemoveHook(update);
	MH_RemoveHook(strict_math_atan2);

	SetWindowLongPtrA(ctx.m_window, -4, reinterpret_cast<long long>(hooked::o_wnd_proc));

	MH_Uninitialize();
}

std::shared_ptr<c_class_loader> c_context::get_class_loader(JNIEnv* _jni)
{
	std::shared_ptr<c_class_loader> ptr;

	switch (m_version)
	{
		case MC_1710:
		{
			switch (ctx.m_client_flavor)
			{
				case VANILLA:
				case BADLION:
				{
					ptr = std::make_shared<c_class_loader_1710>();
					break;
				}

				case FORGE:
				{
					ptr = std::make_shared<c_class_loader_forge_1710>();
					break;
				}

				default:
				{
					return nullptr;
				}
			}

			break;
		}

		case MC_18X:
		{
			switch (ctx.m_client_flavor)
			{
				case VANILLA:
				case BADLION:
				case HYPERIUM:
				{
					ptr = std::make_shared<c_class_loader_18X>();
					break;
				}

				case FORGE:
				{
					ptr = std::make_shared<c_class_loader_forge_18X>();
					break;
				}

				default:
				{
					return nullptr;
				}
			}
			
			break;
		}

		case MC_UNKNOWN:
		default:
		{
			return nullptr;
		}
	}

	ptr->instantiate(_jni);

	return ptr;
}

std::shared_ptr<c_game> c_context::get_game(JNIEnv* _jni)
{
	std::shared_ptr<c_game> ptr;

	switch (m_version)
	{
		case MC_1710:
		{
			switch (ctx.m_client_flavor)
			{
				case VANILLA:
				case BADLION:
				{
					ptr = std::make_shared<c_game_1710>();
					break;
				}

				case FORGE:
				{
					ptr = std::make_shared<c_game_forge_1710>();
					break;
				}

				default:
				{
					return nullptr;
				}
			}
			
			break;
		}

		case MC_18X:
		{
			switch (ctx.m_client_flavor)
			{
				case VANILLA:
				case BADLION:
				case HYPERIUM:
				{
					ptr = std::make_shared<c_game_18X>();
					break;
				}

				case FORGE:
				{
					ptr = std::make_shared<c_game_forge_18X>();
					break;
				}

				default:
				{
					return nullptr;
				}
			}
			
			break;
		}

		case MC_UNKNOWN:
		default:
		{
			return nullptr;
		}
	}

	ptr->instantiate(_jni);

	return ptr;
}

