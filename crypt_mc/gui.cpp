#include "renderer.h"
#include "context.h"

#include "gui.h"

#include <stdio.h>
#include <algorithm>

#include "menu.h"
#include <gl\GL.h>

// symbols often used that are hard to get
// �

c_gui gui;

// wont work cuz menu handles loading/ saving, meaning pos gets set before we can update it
UI::c_window menu(&ctx.m_settings.gui_menu_pos);

void c_gui::background()
{
	// transparent background
	if (menu.data()->m_alpha > 0)
		ctx.m_renderer->draw_filled_rect({ 0.f, 0.f, (float)ctx.m_screen_w, (float)ctx.m_screen_h }, color_t(1, 1, 1, 0.5f * menu.data()->m_alpha));
}


void c_gui::tab_combat()
{
	static UI::c_enable_groupbox auto_clicker_groupbox;
	static UI::c_key_bind auto_clicker_key;
	static UI::c_slider auto_clicker_cps;
	static UI::c_slider auto_clicker_jitter_chance;
	static UI::c_slider auto_clicker_block_delay;
	static UI::c_float_slider auto_clicker_jitter_intensity;
	static UI::c_multi_dropdown auto_clicker_conditions;
	static UI::c_checkbox auto_clicker_item_whitelist;

	static UI::c_enable_groupbox aim_assist_groupbox;
	static UI::c_key_bind aim_assist_key;
	static UI::c_slider aim_assist_fov;
	static UI::c_float_slider aim_assist_v_speed;
	static UI::c_float_slider aim_assist_h_speed;
	static UI::c_float_slider aim_assist_scale;
	static UI::c_float_slider aim_assist_distance;
	static UI::c_multi_dropdown aim_assist_conditions;
	static UI::c_checkbox aim_assist_item_whitelist;

	static UI::c_enable_groupbox velocity_groupbox;
	static UI::c_key_bind velocity_key;
	static UI::c_slider velocity_horizontal;
	static UI::c_slider velocity_vertical;
	static UI::c_slider velocity_chance;
	static UI::c_slider velocity_delay;
	static UI::c_slider velocity_kite;
	static UI::c_multi_dropdown velocity_conditions;

	static UI::c_enable_groupbox reach_groupbox;
	static UI::c_key_bind reach_key;
	static UI::c_float_slider reach_distance;
	static UI::c_slider reach_hitchance;
	static UI::c_float_slider reach_hitboxes_scale;
	static UI::c_multi_dropdown reach_conditions;

	menu.column_left();
	{
		auto_clicker_groupbox.start(menu.data(), xors("auto clicker"));
		{
			if (ctx.m_settings.combat_auto_clicker)
				auto_clicker_key.handle(menu.data(), "", &ctx.m_settings.combat_auto_clicker_key, keytype_t::kt_all);

			auto_clicker_cps.handle(menu.data(), xors("cps"), &ctx.m_settings.combat_auto_clicker_cps, 1, 24, 1, xors("cps"));
			
			if (ctx.m_settings.combat_auto_clicker_jitter)
			{
				auto_clicker_jitter_chance.handle(menu.data(), xors("chance"), &ctx.m_settings.combat_auto_clicker_jitter_chance, 1, 100, 1, xors("%"));
				auto_clicker_jitter_intensity.handle(menu.data(), xors("intensity"), &ctx.m_settings.combat_auto_clicker_jitter_intensity, 0.01f, 1.0f, 0.01f);
			}

			//if (ctx.m_settings.combat_auto_clicker_auto_block)
			//{
			//	auto_clicker_block_delay.handle(menu.data(), xors("delay"), &ctx.m_settings.combat_auto_clicker_block_delay, 1, 1000, 1, xors("ms"));
			//}
		
			auto_clicker_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_auto_clicker_inventory, xors("inventory")},
					{&ctx.m_settings.combat_auto_clicker_break_blocks, xors("break blocks")},
					{&ctx.m_settings.combat_auto_clicker_auto_block, xors("auto block")},
					{&ctx.m_settings.combat_auto_clicker_item_whitelist, xors("item whitelist")},
					{&ctx.m_settings.combat_auto_clicker_jitter, xors("jitter")}
				});
		}
		auto_clicker_groupbox.end(menu.data(), &ctx.m_settings.combat_auto_clicker);

		reach_groupbox.start(menu.data(), xors("reach"));
		{
			if (ctx.m_settings.combat_reach)
				reach_key.handle(menu.data(), "", &ctx.m_settings.combat_reach_key, keytype_t::kt_all);

			reach_distance.handle(menu.data(), xors("distance"), &ctx.m_settings.combat_reach_distance, 3.f, 6.f, 0.01f, xors("%.2f"));
			reach_hitchance.handle(menu.data(), xors("hitchance"), &ctx.m_settings.combat_reach_hitchance, 1, 100, 1, xors("%"));
			
			if (ctx.m_settings.combat_reach_hitboxes)
				reach_hitboxes_scale.handle(menu.data(), xors("hitbox scale"), &ctx.m_settings.combat_reach_hitboxes_scale, 1.f, 5.f, 0.01f, xors("%.2f"));

			reach_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_reach_visible, xors("visible only")},
					{&ctx.m_settings.combat_reach_on_sprint, xors("require sprint")},
					{&ctx.m_settings.combat_reach_disable_in_water, xors("disable in water")},
					{&ctx.m_settings.combat_reach_ycheck, xors("y-check")},
					{&ctx.m_settings.combat_reach_hitboxes, xors("hitboxes")}
				});
		}
		reach_groupbox.end(menu.data(), &ctx.m_settings.combat_reach);

		auto_clicker_key.dropdown(menu.data());
		auto_clicker_conditions.dropdown(menu.data());
		reach_key.dropdown(menu.data());
		reach_conditions.dropdown(menu.data());
	}

	menu.column_right();
	{
		aim_assist_groupbox.start(menu.data(), xors("aim assist"));
		{
			if (ctx.m_settings.combat_aim_assist)
				aim_assist_key.handle(menu.data(), "", &ctx.m_settings.combat_aim_assist_key, keytype_t::kt_all);


			aim_assist_fov.handle(menu.data(), xors("fov"), &ctx.m_settings.combat_aim_assist_fov, 0, 180, 1);
			aim_assist_h_speed.handle(menu.data(), xors("horizontal"), &ctx.m_settings.combat_aim_assist_h_speed, 0.01f, 50.f, 0.01f);

			if (ctx.m_settings.combat_aim_assist_vertical)
				aim_assist_v_speed.handle(menu.data(), xors("vertical"), &ctx.m_settings.combat_aim_assist_v_speed, 0.01f, 50.f, 0.01f);

			aim_assist_distance.handle(menu.data(), xors("distance"), &ctx.m_settings.combat_aim_assist_distance, 3.f, 8.f, 0.01f);
			aim_assist_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_aim_assist_require_click, xors("require click")},
					{&ctx.m_settings.combat_aim_assist_multipoint, xors("multipoint")},
					{&ctx.m_settings.combat_aim_assist_vertical, xors("vertical")},
					{&ctx.m_settings.combat_aim_assist_sticky, xors("sticky")},
					{&ctx.m_settings.combat_aim_assist_break_blocks, xors("break blocks")},
					{&ctx.m_settings.combat_aim_assist_visible_only, xors("visible only")},
					{&ctx.m_settings.combat_aim_assist_item_whitelist, xors("item whitelist")},
					{&ctx.m_settings.combat_aim_assist_invisibles, xors("ignore invisibles")},
					{&ctx.m_settings.combat_aim_assist_nakeds, xors("ignore nakeds")}
				});
		}
		aim_assist_groupbox.end(menu.data(), &ctx.m_settings.combat_aim_assist);

		velocity_groupbox.start(menu.data(), xors("velocity"));
		{
			if (ctx.m_settings.combat_velocity)
				velocity_key.handle(menu.data(), "", &ctx.m_settings.combat_velocity_key, keytype_t::kt_all);

			velocity_horizontal.handle(menu.data(), xors("horizontal"), &ctx.m_settings.combat_velocity_horizontal, 0, 200, 1, xors("%"));
			velocity_vertical.handle(menu.data(), xors("vertical"), &ctx.m_settings.combat_velocity_vertical, 0, 200, 1, xors("%"));

			if (ctx.m_settings.combat_velocity_kite)
				velocity_kite.handle(menu.data(), xors("kite horizontal"), &ctx.m_settings.combat_velocity_kite_val, 100, 300, 1, xors("%"));

			velocity_chance.handle(menu.data(), xors("chance"), &ctx.m_settings.combat_velocity_chance, 0, 100, 1, xors("%"));

			if (ctx.m_settings.combat_velocity_delay)
				velocity_delay.handle(menu.data(), xors("delay"), &ctx.m_settings.combat_velocity_delay_ticks, 0, 8, 1, xors(" ticks"));

		
			velocity_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_velocity_on_sprint, xors("require sprint")},
					{&ctx.m_settings.combat_velocity_require_click, xors("require click")},
					{&ctx.m_settings.combat_velocity_require_target, xors("require target")},
					{&ctx.m_settings.combat_velocity_kite, xors("kite mode")},
					{&ctx.m_settings.combat_velocity_delay, xors("delay")}
				});
		}
		velocity_groupbox.end(menu.data(), &ctx.m_settings.combat_velocity);

		aim_assist_key.dropdown(menu.data());
		aim_assist_conditions.dropdown(menu.data()); 
		velocity_key.dropdown(menu.data());
		velocity_conditions.dropdown(menu.data());
	}
}

void c_gui::tab_player()
{
	static UI::c_enable_groupbox throw_groupbox;

	static UI::c_key_bind throwhealing_key;
	static UI::c_multi_dropdown throwhealing_items;

	static UI::c_key_bind throwpearl_key;

	static UI::c_key_bind throwdebuff_key;
	static UI::c_multi_dropdown throwdebuff_items;

	static UI::c_slider throw_delay;

	static UI::c_enable_groupbox fast_place_groupbox;
	static UI::c_key_bind fast_place_key;
	static UI::c_multi_dropdown fast_place_items;
	static UI::c_slider fast_place_block_delay;
	static UI::c_slider fast_place_projectiles_delay;

	static UI::c_enable_groupbox afk_groupbox;
	static UI::c_slider afk_min_delay;
	static UI::c_slider afk_max_delay;

	menu.column_left();
	{
		throw_groupbox.start(menu.data(), xors("throw"));
		{
			throwhealing_key.handle(menu.data(), xors("healing"), &ctx.m_settings.player_throwhealing_key, keytype_t::kt_toggle);
			throwhealing_items.handle(menu.data(), xors("items"),
				{
					{&ctx.m_settings.player_throwhealing_potions, xors("potions")},
					{&ctx.m_settings.player_throwhealing_soup, xors("soup")}
				});

			throwpearl_key.handle(menu.data(), xors("pearl"), &ctx.m_settings.player_throwpearl_key, keytype_t::kt_toggle);

			throwdebuff_key.handle(menu.data(), xors("debuff"), &ctx.m_settings.player_throwdebuff_key, keytype_t::kt_toggle);
			throwdebuff_items.handle(menu.data(), xors("items"),
				{
					{&ctx.m_settings.player_throwdebuff_damage, xors("damage")},
					{&ctx.m_settings.player_throwdebuff_poison, xors("poison")},
					{&ctx.m_settings.player_throwdebuff_slow, xors("slow")},
					{&ctx.m_settings.player_throwdebuff_weakness, xors("weakness")},
				});

			throw_delay.handle(menu.data(), xors("delay"), &ctx.m_settings.player_throw_delay, 75, 250, 1, xors("ms"));
		}
		throw_groupbox.end(menu.data(), &ctx.m_settings.player_throw);

		throwhealing_key.dropdown(menu.data());
		throwhealing_items.dropdown(menu.data());
		throwpearl_key.dropdown(menu.data());
		throwdebuff_key.dropdown(menu.data());
		throwdebuff_items.dropdown(menu.data());
	}

	menu.column_right();
	{
		fast_place_groupbox.start(menu.data(), xors("fast place"));
		{
			fast_place_key.handle(menu.data(), "", &ctx.m_settings.player_fast_place_key, keytype_t::kt_all);

			if (ctx.m_settings.player_fast_place_blocks)
				fast_place_block_delay.handle(menu.data(), xors("block delay"), &ctx.m_settings.player_fast_place_block_delay, 0, 4, 1);

			if (ctx.m_settings.player_fast_place_projectiles)
				fast_place_projectiles_delay.handle(menu.data(), xors("projectile delay"), &ctx.m_settings.player_fast_place_projectiles_delay, 0, 4, 1);

			fast_place_items.handle(menu.data(), xors("items"),
				{
					{&ctx.m_settings.player_fast_place_blocks, xors("blocks")},
					{&ctx.m_settings.player_fast_place_projectiles, xors("projectiles")}
				});
		}
		fast_place_groupbox.end(menu.data(), &ctx.m_settings.player_fast_place);

		afk_groupbox.start(menu.data(), xors("anti afk"));
		{
			afk_min_delay.handle(menu.data(), xors("min"), &ctx.m_settings.player_anti_afk_min_delay, 30, 300, 1, xors("s"));
			afk_max_delay.handle(menu.data(), xors("max"), &ctx.m_settings.player_anti_afk_max_delay, ctx.m_settings.player_anti_afk_min_delay, 300, 1, xors("s"));

		}
		afk_groupbox.end(menu.data(), &ctx.m_settings.player_anti_afk);

		fast_place_key.dropdown(menu.data());
		fast_place_items.dropdown(menu.data());
	}
}

void c_gui::tab_visuals()
{
	static UI::c_enable_groupbox box_groupbox;
	static UI::c_dropdown box_mode;
	static UI::c_color_picker box_color;
	static UI::c_multi_dropdown box_conditions;
	static UI::c_multi_dropdown box_options;
	static UI::c_color_picker box_filled_color;
	static UI::c_color_picker snaplines_color;
	static UI::c_color_picker outline_color;

	static UI::c_groupbox fullbright_groupbox;
	static UI::c_checkbox fullbright_checkbox;
	static UI::c_key_bind fullbright_key;

	menu.column_left();
	{
		box_groupbox.start(menu.data(), xors("box"));
		{
			box_mode.handle(menu.data(), xors("mode"), { xors("2d"), xors("corner"), xors("3d") }, &ctx.m_settings.visuals_esp_box_mode);
			box_color.handle(menu.data(), xors("color"), ctx.m_settings.visuals_esp_box_color().r_ptr(), ctx.m_settings.visuals_esp_box_color().g_ptr(), ctx.m_settings.visuals_esp_box_color().b_ptr(), ctx.m_settings.visuals_esp_box_color().a_ptr());
			box_conditions.handle(menu.data(), xors("conditions"), {
				{&ctx.m_settings.visuals_esp_invisible, xors("invisible")},
				{&ctx.m_settings.visuals_esp_bots, xors("bots")}
			});

			box_options.handle(menu.data(), xors("options"), {
				{&ctx.m_settings.visuals_esp_names, xors("name")},
				{&ctx.m_settings.visuals_esp_healthbar, xors("healthbar")},
				{&ctx.m_settings.visuals_esp_box_filled, xors("filled")},
				{&ctx.m_settings.visuals_esp_snap_lines, xors("snaplines")},
				{&ctx.m_settings.visuals_esp_box_outlined, xors("outlined")},
			});
			
			if (ctx.m_settings.visuals_esp_box_filled)
				box_filled_color.handle(menu.data(), xors("filled color"), ctx.m_settings.visuals_esp_box_filled_color().r_ptr(), ctx.m_settings.visuals_esp_box_filled_color().g_ptr(), ctx.m_settings.visuals_esp_box_filled_color().b_ptr(), ctx.m_settings.visuals_esp_box_filled_color().a_ptr());
			
			if (ctx.m_settings.visuals_esp_snap_lines)
				snaplines_color.handle(menu.data(), xors("snapline color"), ctx.m_settings.visuals_esp_snap_lines_color().r_ptr(), ctx.m_settings.visuals_esp_snap_lines_color().g_ptr(), ctx.m_settings.visuals_esp_snap_lines_color().b_ptr(), ctx.m_settings.visuals_esp_snap_lines_color().a_ptr());

			if (ctx.m_settings.visuals_esp_box_outlined)
				outline_color.handle(menu.data(), xors("outline color"), ctx.m_settings.visuals_esp_box_outlined_color().r_ptr(), ctx.m_settings.visuals_esp_box_outlined_color().g_ptr(), ctx.m_settings.visuals_esp_box_outlined_color().b_ptr(), ctx.m_settings.visuals_esp_box_outlined_color().a_ptr());
		}
		box_groupbox.end(menu.data(), &ctx.m_settings.visuals_esp_box);

		fullbright_groupbox.start(menu.data(), xors("fullbright"));
		{
			fullbright_checkbox.handle(menu.data(), xors("enable"), &ctx.m_settings.visuals_fullbright);

			if (ctx.m_settings.visuals_fullbright)
				fullbright_key.handle(menu.data(), "", &ctx.m_settings.visuals_fullbright_key, keytype_t::kt_all);
		}
		fullbright_groupbox.end(menu.data());

		box_mode.dropdown(menu.data());
		box_conditions.dropdown(menu.data());
		box_options.dropdown(menu.data());
		fullbright_key.dropdown(menu.data());
	}

	menu.column_right();
	{

	}

	box_color.picker(menu.data());
	box_filled_color.picker(menu.data());
	snaplines_color.picker(menu.data());
	outline_color.picker(menu.data());
}

void c_gui::tab_movement()
{
	static UI::c_enable_groupbox speed_groupbox;
	static UI::c_key_bind speed_key;
	static UI::c_dropdown speed_mode;
	static UI::c_multi_dropdown speed_custom_options;
	static UI::c_float_slider speed_slow_down_ticks;
	static UI::c_float_slider speed_jump_height;
	static UI::c_float_slider speed_ground_speed;
	static UI::c_float_slider speed_air_speed;
	static UI::c_float_slider speed_slow_speed;
	static UI::c_float_slider speed_fall_speed;
	static UI::c_slider speed_slow_ticks;

	static UI::c_enable_groupbox fast_stop_groupbox;
	static UI::c_key_bind fast_stop_key;

	static UI::c_enable_groupbox safe_walk_groupbox;
	static UI::c_key_bind safe_walk_key;

	static UI::c_key_bind longjump_key;
	static UI::c_float_slider longjump_boost;

	static UI::c_enable_groupbox step_groupbox;
	static UI::c_key_bind step_key;
	static UI::c_float_slider step_height;

	static UI::c_enable_groupbox air_control_groupbox;
	static UI::c_key_bind air_control_key;
	static UI::c_float_slider air_control_speed;

	static UI::c_enable_groupbox sprint_groupbox;
	static UI::c_key_bind sprint_key;
	static UI::c_checkbox sprint_omni;

	static UI::c_enable_groupbox flight_groupbox;
	static UI::c_key_bind flight_key;
	static UI::c_dropdown flight_mode;
	static UI::c_float_slider flight_hspeed;
	static UI::c_float_slider flight_vspeed;
	static UI::c_float_slider flight_glide_speed;
	static UI::c_multi_dropdown flight_options;

	menu.column_left();
	{
		flight_groupbox.start(menu.data(), xors("flight"));
		{
			if (ctx.m_settings.movement_flight)
				flight_key.handle(menu.data(), "", &ctx.m_settings.movement_flight_key, keytype_t::kt_all);

			flight_mode.handle(menu.data(), xors("mode"), { xors("hypixel"), xors("bypass"), xors("slow bypass"), xors("normal") }, &ctx.m_settings.movement_flight_mode);

			if (ctx.m_settings.movement_flight_mode != 0) 
			{
				flight_hspeed.handle(menu.data(), xors("horizontal speed"), &ctx.m_settings.movement_flight_hspeed, 0.f, 1.f, 0.01f);
				flight_vspeed.handle(menu.data(), xors("vertical speed"), &ctx.m_settings.movement_flight_vspeed, 0.f, 1.f, 0.01f);
			}
			
			if (ctx.m_settings.movement_flight_glide)
				flight_glide_speed.handle(menu.data(), xors("glide speed"), &ctx.m_settings.movement_flight_glide_speed, 0.f, 1.f, 0.01f);
		
			flight_options.handle(menu.data(), xors("options"),
				{
					{&ctx.m_settings.movement_flight_glide, xors("glide")},
					{&ctx.m_settings.movement_flight_tight, xors("tight")}
				});
		}
		flight_groupbox.end(menu.data(), &ctx.m_settings.movement_flight);

		step_groupbox.start(menu.data(), xors("step"));
		{
			if (ctx.m_settings.movement_step)
				step_key.handle(menu.data(), "", &ctx.m_settings.movement_step_key, keytype_t::kt_all);

			step_height.handle(menu.data(), xors("step height"), &ctx.m_settings.movement_step_height, 0.5f, 5.f, 0.01f);
		}
		step_groupbox.end(menu.data(), &ctx.m_settings.movement_step);

		fast_stop_groupbox.start(menu.data(), xors("fast stop"));
		{
			fast_stop_key.handle(menu.data(), "", &ctx.m_settings.movement_fast_stop_key, keytype_t::kt_all);
		}
		fast_stop_groupbox.end(menu.data(), &ctx.m_settings.movement_fast_stop);

		//safe_walk_groupbox.start(menu.data(), xors("safe walk"));
		//{
		//	safe_walk_key.handle(menu.data(), "", &ctx.m_settings.movement_safe_walk_key, keytype_t::kt_all);
		//}
		//safe_walk_groupbox.end(menu.data(), &ctx.m_settings.movement_safe_walk);

		flight_key.dropdown(menu.data());
		step_key.dropdown(menu.data());
		fast_stop_key.dropdown(menu.data());
		//safe_walk_key.dropdown(menu.data());
		flight_mode.dropdown(menu.data());

		flight_options.dropdown(menu.data());
	}

	menu.column_right();
	{
		speed_groupbox.start(menu.data(), xors("speed"));
		{
			if (ctx.m_settings.movement_speed)
				speed_key.handle(menu.data(), "", &ctx.m_settings.movement_speed_key, keytype_t::kt_all);

			speed_mode.handle(menu.data(), xors("mode"),
				{ xors("bhop"), xors("slowhop"), xors("minihop"), xors("yport"), xors("vanilla"), xors("glidehop"), xors("custom") }, &ctx.m_settings.movement_speed_mode);

			if (ctx.m_settings.movement_speed_mode == 6) {
				speed_custom_options.handle(menu.data(), xors("options"),
					{
						{&ctx.m_settings.movement_speed_custom_slow, xors("slow-down")},
						{&ctx.m_settings.movement_speed_custom_jump_ground, xors("jump")},
						{&ctx.m_settings.movement_speed_custom_jump_height, xors("custom jump-height")},
						{&ctx.m_settings.movement_speed_custom_slow_down, xors("custom slow-speed")},
						{&ctx.m_settings.movement_speed_custom_fall, xors("custom fall-speed")}
					});

				if (ctx.m_settings.movement_speed_custom_slow)
					speed_slow_ticks.handle(menu.data(), xors("slow-down ticks"), &ctx.m_settings.movement_speed_custom_slow_down_ticks, 1, 50, 1);

				if (ctx.m_settings.movement_speed_custom_slow_down)
					speed_slow_speed.handle(menu.data(), xors("slow-down speed"), &ctx.m_settings.movement_speed_custom_slow_speed_val, 0.f, 3.f, 0.01f);

				if (ctx.m_settings.movement_speed_custom_fall)
					speed_fall_speed.handle(menu.data(), xors("fall speed"), &ctx.m_settings.movement_speed_custom_fall_speed_val, 0.f, 3.f, 0.01f);

				if (ctx.m_settings.movement_speed_custom_jump_ground && ctx.m_settings.movement_speed_custom_jump_height)
					speed_jump_height.handle(menu.data(), xors("jump height"), &ctx.m_settings.movement_speed_custom_jump_height_val, 0.01f, 3.f, 0.01f);

				speed_ground_speed.handle(menu.data(), xors("ground speed"), &ctx.m_settings.movement_speed_custom_ground_speed_val, 0.01f, 9.f, 0.01f);
				speed_air_speed.handle(menu.data(), xors("air speed"), &ctx.m_settings.movement_speed_custom_air_speed_val, 0.01f, 9.f, 0.01f);
			}
	
			longjump_key.handle(menu.data(), xors("longjump"), &ctx.m_settings.movement_longjump_key, keytype_t::kt_hold);
			longjump_boost.handle(menu.data(), xors("boost"), &ctx.m_settings.movement_longjump_boost, 0.f, 3.f, 0.01f);
		}
		speed_groupbox.end(menu.data(), &ctx.m_settings.movement_speed);

		sprint_groupbox.start(menu.data(), xors("sprint"));
		{
			sprint_key.handle(menu.data(), "", &ctx.m_settings.movement_sprint_key, keytype_t::kt_all);

		}
		sprint_groupbox.end(menu.data(), &ctx.m_settings.movement_sprint);

		air_control_groupbox.start(menu.data(), xors("air control"));
		{
			air_control_key.handle(menu.data(), "", &ctx.m_settings.movement_air_control_key, keytype_t::kt_all);
			air_control_speed.handle(menu.data(), xors("speed"), &ctx.m_settings.movement_air_control_speed, 0.f, 3.f, 0.01f);
		}
		air_control_groupbox.end(menu.data(), &ctx.m_settings.movement_air_control);

		speed_key.dropdown(menu.data());
		sprint_key.dropdown(menu.data());
		air_control_key.dropdown(menu.data());
		speed_mode.dropdown(menu.data());
		speed_custom_options.dropdown(menu.data());
	}
}

void c_gui::tab_misc()
{
	static UI::c_groupbox menu_setts;
	static UI::c_groupbox hack;
	static UI::c_groupbox global_settings;

	static UI::c_enable_groupbox timer_groupbox;
	static UI::c_key_bind timer_negative_key;
	static UI::c_key_bind timer_positive_key;
	static UI::c_float_slider timer_negative_speed;
	static UI::c_float_slider timer_positive_speed;

	static UI::c_multi_dropdown global_item_whitelist;
	static UI::c_key_bind menu_key;
	static UI::c_color_picker menu_color;
	static UI::c_slider menu_fade_speed;
	static UI::c_checkbox rainbow_mode;

	static UI::c_dropdown restriction;
	static UI::c_dropdown config;
	static UI::c_button save;
	static UI::c_button load;
	static UI::c_button conf_import;
	static UI::c_button conf_export;
	static UI::c_button refresh;
	static UI::c_button deletee;
	static UI::c_label pad;
	static UI::c_text_input config_name;
	static char cfg_name_str[32]{};
	static UI::c_button create;
	static UI::c_button defaulter;
	static UI::c_button uninject;

	menu.column_left();
	{
		menu_setts.start(menu.data(), xors("hack"));
		{
			menu_key.handle(menu.data(), xors("key"), &ctx.m_settings.menu_key, keytype_t::kt_toggle);

			menu_color.handle(menu.data(), xors("accent"), ctx.m_settings.gui_accent_color().r_ptr(), ctx.m_settings.gui_accent_color().g_ptr(), ctx.m_settings.gui_accent_color().b_ptr(), ctx.m_settings.gui_accent_color().a_ptr());
			menu_fade_speed.handle(menu.data(), xors("fade speed"), &ctx.m_settings.gui_fade_speed, 1, 50);

			rainbow_mode.handle(menu.data(), xors("rainbow mode"), &ctx.m_settings.gui_rainbow);

			uninject.handle(menu.data(), xors("unload"), []
				{
					ctx.m_panic = true;
				});
		}
		menu_setts.end(menu.data());

		global_settings.start(menu.data(), xors("global options"));
		{
			global_item_whitelist.handle(menu.data(), xors("item whitelist"),
				{
					{&ctx.m_settings.global_setting_swords, xors("swords")},
					{&ctx.m_settings.global_setting_axes, xors("axes")},
					{&ctx.m_settings.global_setting_hoes, xors("hoes")},
					{&ctx.m_settings.global_setting_pick_axes, xors("pickaxes")},
					{&ctx.m_settings.global_setting_shovels, xors("shovels")},
					{&ctx.m_settings.global_setting_fishing_rod, xors("fishing rods")}
				});	
		}
		global_settings.end(menu.data());

		timer_groupbox.start(menu.data(), xors("timer"));
		{
			if (ctx.m_settings.misc_timer)
				timer_positive_key.handle(menu.data(), "positive", &ctx.m_settings.misc_positive_timer_key, keytype_t::kt_all);
			
			timer_positive_speed.handle(menu.data(), xors("positive speed"), &ctx.m_settings.misc_timer_positive_speed, 1.f, 5.f, 0.01f);

			if (ctx.m_settings.misc_timer)
				timer_negative_key.handle(menu.data(), "negative", &ctx.m_settings.misc_negative_timer_key, keytype_t::kt_all);

			timer_negative_speed.handle(menu.data(), xors("negative speed"), &ctx.m_settings.misc_timer_negative_speed, .01f, 1.f, 0.01f);
		}
		timer_groupbox.end(menu.data(), &ctx.m_settings.misc_timer);

		restriction.dropdown(menu.data());
		global_item_whitelist.dropdown(menu.data());
		timer_positive_key.dropdown(menu.data());
		timer_negative_key.dropdown(menu.data());
	}

	menu.column_right();
	{
		hack.start(menu.data(), xors("config"));
		{
			// le segfaulter
			config.handle(menu.data(), xors(""), ctx.m_cfg_list, &ctx.m_current_cfg, 5);
			save.handle(menu.data(), xors("save"), []
				{
					ctx.m_settings.save((ctx.m_cfg_list[ctx.m_current_cfg] + xors(".crypt")).c_str());
				});

			load.handle(menu.data(), xors("load"), []
				{
					ctx.m_settings.load((ctx.m_cfg_list[ctx.m_current_cfg] + xors(".crypt")).c_str());
				});

			conf_import.handle(menu.data(), xors("import"), []
				{
					if (!OpenClipboard(ctx.m_window))
						return;

					const auto hwnd_clip = GetClipboardData(CF_TEXT);
					if (!hwnd_clip)
						return;

					char* text_str = static_cast<char*>(GlobalLock(hwnd_clip));
					if (!text_str)
						return;

					std::string config(text_str);
					config = util::base64::base64_decode(config);
					ctx.m_settings.load_conf(config);

					GlobalUnlock(hwnd_clip);
					CloseClipboard();
				});

			conf_export.handle(menu.data(), xors("export"), []
				{
					std::string config{};
					ctx.m_settings.save_conf(config);
					config = util::base64::base64_encode((const uint8_t*)config.c_str(), config.length());
				
					const HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, config.length());
					memcpy(GlobalLock(mem), config.c_str(), config.length());
					GlobalUnlock(mem);
					if (!OpenClipboard(nullptr))
						return;
				
					EmptyClipboard();
					if (!SetClipboardData(CF_TEXT, mem))
						return;
				
					CloseClipboard();
				});

			deletee.handle(menu.data(), xors("delete"), []
				{
					const auto config_hash = fnvr(ctx.m_cfg_list[ctx.m_current_cfg].c_str());
					const auto default_hash = fnvc("default");
					if (config_hash == default_hash)
						return;

					const std::string config = std::string(ctx.m_cfg_list[ctx.m_current_cfg] + xors(".crypt"));

					auto key = util::open_reg_key(xors("Software\\Spotify"));
					util::delete_reg_key(key, config.c_str());

					ctx.m_cfg_list.erase(ctx.m_cfg_list.begin() + ctx.m_current_cfg);
					ctx.m_current_cfg = ctx.m_cfg_list.size() - 1;
				});

			defaulter.handle(menu.data(), xors("default"), []
				{
					std::string config(ctx.m_cfg_list[ctx.m_current_cfg] + xors(".crypt"));

					auto key = util::open_reg_key(xors("Software\\Spotify"));
					util::set_reg_key(key, config.c_str(), (void*)"", 1);
					ctx.m_settings.load(config.c_str());
				});

			pad.handle(menu.data(), "");
			config_name.handle(menu.data(), xors("new config"), cfg_name_str);
			create.handle(menu.data(), xors("create"), []
				{
					if (strlen(cfg_name_str) < 1 || !util::valid_filename(cfg_name_str))
						return;

					for (size_t i = 0; i < ctx.m_cfg_list.size(); ++i)
					{
						// gotta make sure there are no issues with for example trying to create "nigga1" when theres already "nigga" etc
						if (strlen(cfg_name_str) == strlen(ctx.m_cfg_list[i].c_str()) && strncmp(ctx.m_cfg_list[i].c_str(), cfg_name_str, ctx.m_cfg_list[i].size()) == 0)
							return;
					}

					// create new config
					ctx.m_cfg_list.push_back(cfg_name_str);
					auto key = util::open_reg_key(xors("Software\\Spotify"));
					util::set_reg_key(key, (std::string(cfg_name_str) + std::string(xors(".crypt"))).c_str(), (void*)"", 1);

					// set current cfg to newest, size counts from 1
					ctx.m_current_cfg = ctx.m_cfg_list.size() - 1;

					RtlSecureZeroMemory(cfg_name_str, sizeof(cfg_name_str));
				});

			hack.end(menu.data());
			config.dropdown(menu.data());
		}
	}

	menu_color.picker(menu.data());
}

void c_gui::draw()
{
	if (!menu.handle(m_tabs, color_t(ctx.m_settings.gui_accent_color().r(), ctx.m_settings.gui_accent_color().g(), ctx.m_settings.gui_accent_color().b())))
		return;

	switch (menu.m_current_tab)
	{
		case TABS_COMBAT:
		{
			tab_combat();
			break;
		}
		
		case TABS_PLAYER:
		{
			tab_player();
			break;
		}
		
		case TABS_MOVEMENT:
		{
			tab_movement();
			break;
		}
		
		case TABS_VISUALS:
		{
			tab_visuals();
			break;
		}
		
		case TABS_MISC:
		{
			tab_misc();
			break;
		}
		
		default:
			break;
	}
}