#include "renderer.h"
#include "context.h"

#include "gui.h"

#include <stdio.h>
#include <algorithm>

#include "menu.h"

// symbols often used that are hard to get
// �

c_gui gui;

// wont work cuz menu handles loading/ saving, meaning pos gets set before we can update it
UI::c_window menu(&ctx.m_settings.gui_menu_pos);
UI::c_cursor cursor;

void c_gui::background()
{
	// transparent background
	ctx.m_renderer->draw_filled_rect({ 0.f, 0.f, (float)ctx.m_screen_w, (float)ctx.m_screen_h }, color_t(1, 1, 1, 0.5f * menu.data()->m_alpha));
}

void c_gui::tab_combat()
{
	static UI::c_enable_groupbox auto_clicker_groupbox;
	static UI::c_key_bind auto_clicker_key;
	static UI::c_slider auto_clicker_min_cps;
	static UI::c_slider auto_clicker_max_cps;
	static UI::c_multi_dropdown auto_clicker_conditions;

	static UI::c_enable_groupbox aim_assist_groupbox;
	static UI::c_key_bind aim_assist_key;
	static UI::c_slider aim_assist_fov;
	static UI::c_float_slider aim_assist_v_speed;
	static UI::c_float_slider aim_assist_h_speed;
	static UI::c_float_slider aim_assist_distance;
	static UI::c_multi_dropdown aim_assist_conditions;

	static UI::c_enable_groupbox velocity_groupbox;
	static UI::c_key_bind velocity_key;;
	static UI::c_slider velocity_horizontal;
	static UI::c_slider velocity_vertical;
	static UI::c_slider velocity_chance;
	static UI::c_slider velocity_delay;
	static UI::c_multi_dropdown velocity_conditions;

	static UI::c_enable_groupbox reach_groupbox;
	static UI::c_key_bind reach_key;
	static UI::c_float_slider reach_distance_min;
	static UI::c_float_slider reach_distance_max;
	static UI::c_multi_dropdown reach_conditions;

	menu.column_left();
	{
		auto_clicker_groupbox.start(menu.data(), xors("auto clicker"));
		{
			if (ctx.m_settings.combat_auto_clicker)
				auto_clicker_key.handle(menu.data(), "", &ctx.m_settings.combat_auto_clicker_key, keytype_t::kt_all);

			auto_clicker_min_cps.handle(menu.data(), xors("min"), &ctx.m_settings.combat_auto_clicker_min_cps, 0, 24, 1, xors("cps"));
			auto_clicker_max_cps.handle(menu.data(), xors("max"), &ctx.m_settings.combat_auto_clicker_max_cps, ctx.m_settings.combat_auto_clicker_min_cps, 25, 1, xors("cps"));
			auto_clicker_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_auto_clicker_weapons_only, xors("weapons only")},
					{&ctx.m_settings.combat_auto_clicker_inventory, xors("inventory")},
					{&ctx.m_settings.combat_auto_clicker_block_hit, xors("block hit")}
				});
		}
		auto_clicker_groupbox.end(menu.data(), &ctx.m_settings.combat_auto_clicker);

		reach_groupbox.start(menu.data(), xors("reach"));
		{
			if (ctx.m_settings.combat_reach)
				reach_key.handle(menu.data(), "", &ctx.m_settings.combat_reach_key, keytype_t::kt_all);

			reach_distance_min.handle(menu.data(), xors("min"), &ctx.m_settings.combat_reach_distance_min, 3.f, 5.99f, 0.1f, xors("%.2f"));
			reach_distance_max.handle(menu.data(), xors("max"), &ctx.m_settings.combat_reach_distance_max, ctx.m_settings.combat_reach_distance_min, 6.f, 0.1f, xors("%.2f"));
			reach_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_reach_on_sprint, xors("on sprint")},
					{&ctx.m_settings.combat_reach_disable_in_water, xors("disable in water")},
					{&ctx.m_settings.combat_reach_y_check, xors("y check")}
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
			aim_assist_h_speed.handle(menu.data(), xors("horizontal"), &ctx.m_settings.combat_aim_assist_h_speed, 1.f, 10.f, 0.01f);

			if (ctx.m_settings.combat_aim_assist_vertical)
				aim_assist_v_speed.handle(menu.data(), xors("vertical"), &ctx.m_settings.combat_aim_assist_v_speed, 1.f, 10.f, 0.01f);

			aim_assist_distance.handle(menu.data(), xors("distance"), &ctx.m_settings.combat_aim_assist_distance, 3.f, 6.f, 0.01f);
			aim_assist_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_aim_assist_require_click, xors("require click")},
					{&ctx.m_settings.combat_aim_assist_weapons_only, xors("weapons only")},
					{&ctx.m_settings.combat_aim_assist_vertical, xors("vertical")},
					{&ctx.m_settings.combat_aim_assist_sticky, xors("sticky")},
					{&ctx.m_settings.combat_aim_assist_invisibles, xors("invisibles")},
					{&ctx.m_settings.combat_aim_assist_nakeds, xors("nakeds")}
				});
		}
		aim_assist_groupbox.end(menu.data(), &ctx.m_settings.combat_aim_assist);

		velocity_groupbox.start(menu.data(), xors("velocity"));
		{
			if (ctx.m_settings.combat_velocity)
				velocity_key.handle(menu.data(), "", &ctx.m_settings.combat_velocity_key, keytype_t::kt_all);

			velocity_horizontal.handle(menu.data(), xors("horizontal"), &ctx.m_settings.combat_velocity_horizontal, 0, 200, 1, xors("%"));
			velocity_vertical.handle(menu.data(), xors("vertical"), &ctx.m_settings.combat_velocity_vertical, 0, 200, 1, xors("%"));
			velocity_chance.handle(menu.data(), xors("chance"), &ctx.m_settings.combat_velocity_chance, 0, 100, 1, xors("%"));

			if (ctx.m_settings.combat_velocity_delay)
				velocity_delay.handle(menu.data(), xors("delay"), &ctx.m_settings.combat_velocity_delay_ticks, 0, 8, 1, xors(" ticks"));

			velocity_conditions.handle(menu.data(), xors("conditions"),
				{
					{&ctx.m_settings.combat_velocity_on_sprint, xors("on sprint")},
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
	static UI::c_enable_groupbox throwpot_groupbox;
	static UI::c_key_bind throwpot_key;
	static UI::c_slider throwpot_delay;
	static UI::c_slider throwpot_health;
	static UI::c_multi_dropdown throwpot_conditions;

	static UI::c_enable_groupbox fast_place_groupbox;
	static UI::c_key_bind fast_place_key;

	static UI::c_groupbox afk_groupbox;
	static UI::c_checkbox afk;

	menu.column_left();
	{
		throwpot_groupbox.start(menu.data(), xors("throwpot"));
		{
			throwpot_key.handle(menu.data(), "", &ctx.m_settings.combat_throwpot_key, keytype_t::kt_hold);
			throwpot_delay.handle(menu.data(), xors("delay"), &ctx.m_settings.combat_throwpot_delay, 1, 200, 1, xors("ms"));
			throwpot_health.handle(menu.data(), xors("health"), &ctx.m_settings.combat_throwpot_health, 1, 20, 1, xors("hearts"));

			throwpot_conditions.handle(menu.data(), xors("items"),
				{
					{&ctx.m_settings.combat_throwpot_potions, xors("potions")},
					{&ctx.m_settings.combat_throwpot_soup, xors("soup")}
				});
		}
		throwpot_groupbox.end(menu.data(), &ctx.m_settings.combat_throwpot);

		throwpot_key.dropdown(menu.data());
		throwpot_conditions.dropdown(menu.data());
	}

	menu.column_right();
	{
		fast_place_groupbox.start(menu.data(), xors("fast place"));
		{
			fast_place_key.handle(menu.data(), "", &ctx.m_settings.player_fast_place_key, keytype_t::kt_all);
		}
		fast_place_groupbox.end(menu.data(), &ctx.m_settings.player_fast_place);

		afk_groupbox.start(menu.data(), xors("anti afk"));
		{
			afk.handle(menu.data(), xors("enable"), &ctx.m_settings.player_anti_afk);
		}
		afk_groupbox.end(menu.data());

		fast_place_key.dropdown(menu.data());
	}
}

void c_gui::tab_visuals()
{
	static UI::c_groupbox box_groupbox;
	static UI::c_dropdown box;
	static UI::c_color_picker box_color;
	static UI::c_slider box_color_alpha;
	static UI::c_checkbox box_filled;
	static UI::c_color_picker box_outline_color;
	static UI::c_slider box_outline_color_alpha;
	static UI::c_checkbox snap_lines;
	static UI::c_color_picker snap_lines_color;
	static UI::c_slider snap_lines_color_alpha;

	static UI::c_checkbox name_tags;

	menu.column_left();
	{
		box_groupbox.start(menu.data(), xors("box"));
		{
			box.handle(menu.data(), xors("mode"), { xors("none"), xors("2d"), xors("3d") }, &ctx.m_settings.visuals_esp_box);
			box_color.handle(menu.data(), xors("color"), ctx.m_settings.visuals_esp_box_color().r_ptr(), ctx.m_settings.visuals_esp_box_color().g_ptr(), ctx.m_settings.visuals_esp_box_color().b_ptr());
			box_color_alpha.handle(menu.data(), "", reinterpret_cast<int*>(ctx.m_settings.visuals_esp_box_color().a_ptr()), 0, 255, 1);
			box_filled.handle(menu.data(), xors("filled"), &ctx.m_settings.visuals_esp_box_filled);
			snap_lines.handle(menu.data(), xors("snap lines"), &ctx.m_settings.visuals_esp_snap_lines);

			if (ctx.m_settings.visuals_esp_box_filled)
			{
				box_outline_color.handle(menu.data(), xors("color"), ctx.m_settings.visuals_esp_box_outline_color().r_ptr(), ctx.m_settings.visuals_esp_box_outline_color().g_ptr(), ctx.m_settings.visuals_esp_box_outline_color().b_ptr());
				box_outline_color_alpha.handle(menu.data(), "", reinterpret_cast<int*>(ctx.m_settings.visuals_esp_box_outline_color().a_ptr()), 0, 255, 1);
			}

			if (ctx.m_settings.visuals_esp_snap_lines)
			{
				snap_lines_color.handle(menu.data(), xors("color"), ctx.m_settings.visuals_esp_snap_lines_color().r_ptr(), ctx.m_settings.visuals_esp_snap_lines_color().g_ptr(), ctx.m_settings.visuals_esp_snap_lines_color().b_ptr());
				snap_lines_color_alpha.handle(menu.data(), "", reinterpret_cast<int*>(ctx.m_settings.visuals_esp_snap_lines_color().a_ptr()), 0, 255, 1);
			}
		}
		box_groupbox.end(menu.data());

		box.dropdown(menu.data());
	}

	menu.column_right();
	{
		name_tags.handle(menu.data(), xors("nametags"), &ctx.m_settings.visuals_esp_nametags);
	}

	box_color.picker(menu.data());
	box_outline_color.picker(menu.data());
}

void c_gui::tab_movement()
{
	static UI::c_enable_groupbox timer_groupbox;
	static UI::c_key_bind timer_key;
	static UI::c_float_slider timer_speed;

	static UI::c_enable_groupbox speed_groupbox;
	static UI::c_key_bind speed_key;
	static UI::c_dropdown speed_mode;

	static UI::c_key_bind longjump_key;
	static UI::c_float_slider longjump_boost;

	static UI::c_enable_groupbox step_groupbox;
	static UI::c_key_bind step_key;
	static UI::c_float_slider step_height;

	static UI::c_enable_groupbox air_control_groupbox;
	static UI::c_key_bind air_control_key;

	static UI::c_enable_groupbox sprint_groupbox;
	static UI::c_key_bind sprint_key;

	static UI::c_enable_groupbox flight_groupbox;
	static UI::c_key_bind flight_key;
	static UI::c_float_slider flight_hspeed;
	static UI::c_float_slider flight_vspeed;
	static UI::c_float_slider flight_glide_speed;
	static UI::c_multi_dropdown flight_options;

	menu.column_left();
	{
		timer_groupbox.start(menu.data(), xors("timer"));
		{
			if (ctx.m_settings.movement_timer)
				timer_key.handle(menu.data(), "", &ctx.m_settings.movement_timer_key, keytype_t::kt_all);

			timer_speed.handle(menu.data(), xors("speed"), &ctx.m_settings.movement_timer_speed, .01f, 3.f, 0.01f);
		}
		timer_groupbox.end(menu.data(), &ctx.m_settings.movement_timer);

		flight_groupbox.start(menu.data(), xors("flight"));
		{
			if (ctx.m_settings.movement_flight)
				flight_key.handle(menu.data(), "", &ctx.m_settings.movement_flight_key, keytype_t::kt_all);

			flight_hspeed.handle(menu.data(), xors("horizontal speed"), &ctx.m_settings.movement_flight_hspeed, 0.f, 1.f, 0.01f);
			flight_vspeed.handle(menu.data(), xors("vertical speed"), &ctx.m_settings.movement_flight_vspeed, 0.f, 1.f, 0.01f);
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

		timer_key.dropdown(menu.data());
		flight_key.dropdown(menu.data());
		step_key.dropdown(menu.data());


		flight_options.dropdown(menu.data());
	}

	menu.column_right();
	{
		speed_groupbox.start(menu.data(), xors("speed"));
		{
			if (ctx.m_settings.movement_speed)
				speed_key.handle(menu.data(), "", &ctx.m_settings.movement_speed_key, keytype_t::kt_all);

			speed_mode.handle(menu.data(), xors("mode"),
				{ xors("bhop"), xors("slowhop"), xors("minihop"), xors("yport"), xors("vanilla"), xors("glidehop") }, &ctx.m_settings.movement_speed_mode);

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
		}
		air_control_groupbox.end(menu.data(), &ctx.m_settings.movement_air_control);

		speed_key.dropdown(menu.data());
		sprint_key.dropdown(menu.data());
		air_control_key.dropdown(menu.data());
		speed_mode.dropdown(menu.data());
	}
}

void c_gui::tab_config()
{
	static UI::c_groupbox menu_setts;
	static UI::c_groupbox hack;

	static UI::c_key_bind menu_key;
	static UI::c_color_picker menu_color;
	static UI::c_slider menu_fade_speed;
	static UI::c_checkbox rainbow_mode;

	static UI::c_dropdown restriction;
	static UI::c_dropdown config;
	static UI::c_button save;
	static UI::c_button load;
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

			menu_color.handle(menu.data(), xors("accent"), ctx.m_settings.gui_accent_color().r_ptr(), ctx.m_settings.gui_accent_color().g_ptr(), ctx.m_settings.gui_accent_color().b_ptr());
			menu_fade_speed.handle(menu.data(), xors("fade speed"), &ctx.m_settings.gui_fade_speed, 0, 15);

			rainbow_mode.handle(menu.data(), xors("rainbow mode"), &ctx.m_settings.gui_rainbow);

			uninject.handle(menu.data(), xors("unload"), []
				{
					ctx.m_panic = true;
				});

			menu_setts.end(menu.data());
			restriction.dropdown(menu.data());
		}
	}

	menu.column_right();
	{
		hack.start(menu.data(), xors("config"));
		{
			// le segfaulter
			config.handle(menu.data(), xors(""), ctx.m_cfg_list, &ctx.m_current_cfg, 5);
			save.handle(menu.data(), xors("save"), []
				{
					ctx.m_settings.save((std::string(xors("./")) + ctx.m_cfg_list[ctx.m_current_cfg] + xors(".cfg")).c_str());
				});

			load.handle(menu.data(), xors("load"), []
				{
					ctx.m_settings.load((std::string(xors("./")) + ctx.m_cfg_list[ctx.m_current_cfg] + xors(".cfg")).c_str());
				});

			refresh.handle(menu.data(), xors("refresh"), []
				{
					ctx.m_cfg_list.clear();

					// anti segfault
					// nothing but trouble
					util::create_file(xors("./default.cfg"), OPEN_EXISTING);
					util::create_file(xors("./skins.cfg"), OPEN_EXISTING);
					ctx.m_cfg_list.push_back(xors("default"));
					//ctx.m_cfg_list.push_back( xors( "skins" ) );
					util::get_all_files(ctx.m_cfg_list, xors("./"));
				});

			deletee.handle(menu.data(), xors("delete"), []
				{
					if (fnvr(ctx.m_cfg_list[ctx.m_current_cfg].c_str()) == fnvc("default"))
						return;

					const std::string file = std::string(xors("./")) + ctx.m_cfg_list[ctx.m_current_cfg] + xors(".cfg");

					// doesnt work after deleting a file once // what?
					std::remove(file.c_str());

					ctx.m_cfg_list.erase(ctx.m_cfg_list.begin() + ctx.m_current_cfg);
					ctx.m_current_cfg = ctx.m_cfg_list.size() - 1;
				});

			defaulter.handle(menu.data(), xors("default"), []
				{
					// GRAB DA JIMMY OUT THE GLOVE BOX	
					std::ofstream cfg;
					cfg.open((std::string(xors("./")) + ctx.m_cfg_list[ctx.m_current_cfg] + xors(".cfg")).c_str(), std::ofstream::out);
					cfg.clear();
					cfg.close();
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
					util::create_file((std::string(xors("./")) + cfg_name_str + std::string(xors(".cfg"))).c_str(), CREATE_NEW, GENERIC_READ | GENERIC_WRITE);

					// set current cfg to newest, size counts from 1
					ctx.m_current_cfg = ctx.m_cfg_list.size() - 1;

					memset(cfg_name_str, 0, sizeof(cfg_name_str));
				});

			hack.end(menu.data());
			config.dropdown(menu.data());
		}
	}

	menu_color.picker(menu.data());
}

void c_gui::draw()
{
	ctx.m_menu_open = menu.is_open();

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
		
		case TABS_CONFIG:
		{
			tab_config();
			break;
		}
		
		default:
			break;
	}

	cursor.handle();
}