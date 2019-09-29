#include "context.h"
#include "auto_clicker.h"
#include <random>

bool left_click = true, set_change = false, blocking = false;

int current_delay = 0;

int min_cps = ctx.m_settings.combat_auto_clicker_min_cps, max_cps = ctx.m_settings.combat_auto_clicker_max_cps;
int clicks_skip = 0, clicks_until_skip = 38, skip_click_amt = 1, skip_click_count = 0;
int clicks_change = 0, clicks_until_change = 50, clicks_until_escape = 12;
float multiplier = 0.5;

uint64_t last_click = 0;

void c_auto_clicker::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (ctx.m_menu_open)
		return;

	const bool in_inventory = mc->is_in_inventory();
	const bool has_focus = mc->in_game_has_focus();

	if (!in_inventory && !has_focus)
		return;

	if (!ctx.m_settings.combat_auto_clicker_inventory && in_inventory)
		return;

	bool white_list = false;

	if (ctx.m_settings.combat_auto_clicker_item_whitelist && !in_inventory)
	{
		if (ctx.m_settings.global_setting_axes && self->holding_axe())
			white_list = true;

		if (ctx.m_settings.global_setting_swords && self->holding_sword())
			white_list = true;

		if (ctx.m_settings.global_setting_hoes && self->holding_hoe())
			white_list = true;

		if (ctx.m_settings.global_setting_shovels && self->holding_shovel())
			white_list = true;

		if (ctx.m_settings.global_setting_pick_axes && self->holding_pick_axe())
			white_list = true;

		if (ctx.m_settings.global_setting_fishing_rod && self->holding_fishing_rod())
			white_list = true;

		if (!white_list)
			return;
	}

	if (in_inventory && ctx.m_settings.combat_auto_clicker_inventory && !g_input.is_key_pressed(KEYS_SHIFT))
		return;

	const auto mouse_down = GetAsyncKeyState(VK_LBUTTON) < 0;

	const auto mouse_r_down = GetAsyncKeyState(VK_RBUTTON) < 0;

	static bool init_block_break = false;

	if (ctx.m_settings.combat_auto_clicker_break_blocks && mc->is_hovering_block() && !in_inventory)
	{
		left_click = true;
		current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-(((max_cps - min_cps) + util::random(-20, 20))), ((max_cps - min_cps) + util::random(-20, 20))), 25, 2000);
		last_click = GetTickCount64();
		clicks_skip = 0;
		clicks_until_skip = util::random(62, 81) + ctx.m_settings.combat_auto_clicker_max_cps;

		if (mouse_down)
		{
			if (!init_block_break)
			{
				init_block_break = true;

				g_input.press_mouse(true);
			}
		}
		else
			init_block_break = false;

		return;
	}
	else
		init_block_break = false;
	

	if (mouse_down)
	{
		if (ctx.m_settings.combat_auto_clicker_jitter && util::random(0, 100) <= ctx.m_settings.combat_auto_clicker_jitter_chance)
		{
			float sens = mc->get_mouse_sensitivity() * 0.6f + 0.2f;
			float step = pow(sens, 3) * 8.0f;

			auto random_h = util::random(-((float)ctx.m_settings.combat_auto_clicker_jitter_intensity), (float)ctx.m_settings.combat_auto_clicker_jitter_intensity);
			auto random_v = util::random(-((float)ctx.m_settings.combat_auto_clicker_jitter_intensity), (float)ctx.m_settings.combat_auto_clicker_jitter_intensity);

			self->set_yaw(self->get_yaw() + util::convert_legit_value(step, random_h, util::random(1, 3)));
			self->set_pitch(self->get_pitch() + util::convert_legit_value(step, random_v, util::random(1, 3)));
		}

		if (in_inventory)
		{
			bool ret = true;

			for (auto i = 36; i < 45; ++i)
				if (!self->get_stack(i))
					ret = false;

			if (ret)
			{
				left_click = true;
				current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-(((max_cps - min_cps) + util::random(-20, 20))), ((max_cps - min_cps) + util::random(-20, 20))), 10, 2000);
				last_click = GetTickCount64();
				return;
			}
		}

		if (last_click == 0)
			last_click = GetTickCount64();

		if (clicks_change > clicks_until_change)
		{
			if (!set_change)
			{
				const float random = util::random(1.085f, 1.155f);
		
				const float rand_multiplier = util::random(0, 100) > 70 ? 1.f : -1.f;
		
				min_cps = abs(ctx.m_settings.combat_auto_clicker_min_cps * std::copysign(random, rand_multiplier));
				max_cps = abs(ctx.m_settings.combat_auto_clicker_max_cps * std::copysign(random, rand_multiplier));
		
				set_change = true;
			}
		
			if (clicks_change - clicks_until_change > clicks_until_escape)
			{
				clicks_change = 0;
				clicks_until_change = util::random(75, 95);
				clicks_until_escape = util::random(4, 9);
			}
		}
		else
		{
			min_cps = ctx.m_settings.combat_auto_clicker_min_cps;
			max_cps = ctx.m_settings.combat_auto_clicker_max_cps + 1;
		
			set_change = false;
		}

		if (clicks_skip > clicks_until_skip)
		{
			if (GetTickCount64() - last_click > 80)
			{
				skip_click_count++;

				if (skip_click_count >= skip_click_amt)
				{
					clicks_skip = 0;
					clicks_until_skip = util::random(62, 81) + ctx.m_settings.combat_auto_clicker_max_cps;
					skip_click_amt = util::random(2, 3);
					skip_click_count = 0;
				}

				left_click = true;
				current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-(((max_cps - min_cps) + util::random(-20, 20))), ((max_cps - min_cps) + util::random(-20, 20))), 25, 2000);
				last_click = GetTickCount64();
			}
			return;
		}

		if (mouse_r_down && ctx.m_settings.combat_auto_clicker_block_hit)
		{
			if (((GetTickCount64() - last_click) <= current_delay * 0.1f && (GetTickCount64() - last_click) >= current_delay * 0.025f) || (GetTickCount64() - last_click <= current_delay * 0.75 && GetTickCount64() - last_click > current_delay * 0.7))
			{
				if (!blocking)
				{
					blocking = true;

					g_input.press_mouse(false);
				}
			}
			else if (blocking)
			{
				blocking = false;

				g_input.release_mouse(false);
			}
		}

		if (left_click && ((GetTickCount64() - last_click) > util::random(static_cast<int>(current_delay * multiplier), static_cast<int>(current_delay * multiplier) + current_delay / util::random(9, 21))))
		{
			if (blocking)
				g_input.release_mouse(false);

			g_input.press_mouse(true);
			left_click = false;
			multiplier = util::random(0.4, 0.6);
		}
		else if (!left_click && GetTickCount64() - last_click > current_delay)
		{
			g_input.release_mouse(true);
			left_click = true;
			current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-(((max_cps - min_cps) + util::random(-20, 20))), ((max_cps - min_cps) + util::random(-20, 20))), 25, 2000);
			last_click = GetTickCount64();
			clicks_skip += 1;
			clicks_change += 1;
		}
		return;
	}

	last_click = 0;
}