#include "context.h" 
#include "auto_clicker.h" 
#include <random> 

bool left_click = true, set_change = false, blocking = false;

int min_cps = ctx.m_settings.combat_auto_clicker_min_cps, max_cps = ctx.m_settings.combat_auto_clicker_max_cps;
int clicks_skip = 0, clicks_until_skip = 40, skip_click_amt = 4, skip_click_count = 0, clicks_change = 0, clicks_until_change = 50, clicks_until_escape = 9, current_delay = 0, clicks = 0;
int max_rand = 0;
float multiplier = 0.5;

uint64_t last_click = 0;

void c_auto_clicker::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	min_cps = ctx.m_settings.combat_auto_clicker_min_cps;
	max_cps = ctx.m_settings.combat_auto_clicker_max_cps;
	if (ctx.m_menu_open)
		return;

	if (m_current_delay == 0)
		reset();

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

	const auto left_mouse_down = GetAsyncKeyState(VK_LBUTTON) < 0;
	const auto right_mouse_down = GetAsyncKeyState(VK_RBUTTON) < 0;

	static bool init_block_break = false;

	if (ctx.m_settings.combat_auto_clicker_break_blocks && mc->is_hovering_block() && !in_inventory)
	{
		reset();

		if (left_mouse_down)
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


	if (left_mouse_down)
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
				reset();
				return;
			}
		}

		if (m_last_click == 0)
			m_last_click = GetTickCount64();

		//if (right_mouse_down && ctx.m_settings.combat_auto_clicker_block_hit)
		//{
		//	if (((GetTickCount64() - last_click) <= current_delay * 0.1f && (GetTickCount64() - last_click) >= current_delay * 0.025f) || (GetTickCount64() - last_click <= current_delay * 0.75 && GetTickCount64() - last_click > current_delay * 0.7))
		//	{
		//		if (!blocking)
		//		{
		//			blocking = true;
		//
		//			g_input.press_mouse(false);
		//		}
		//	}
		//	else if (blocking)
		//	{
		//		blocking = false;
		//
		//		g_input.release_mouse(false);
		//	}
		//}


		if (left_click && ((GetTickCount64() - last_click) > current_delay * multiplier))
		{
			if (blocking)
				g_input.release_mouse(false);

			g_input.press_mouse(true);
			left_click = false;
			multiplier = util::random(0.45, 0.55);
		}
		else if (!m_left_click && GetTickCount64() - m_last_click > m_current_delay)
		{
			g_input.release_mouse(true);

			m_clicks += 1;

			reset();

			m_clicks_skip += 1;
			m_clicks_change += 1;
		}
		return;
	}

	m_last_click = 0;
}

void c_auto_clicker::reset()
{
	auto chance_one = util::random(0, 50);
	auto chance_two = util::random(0, 50);
	auto chance_final = chance_one + chance_two;


	auto tick = util::random(0, 100) < 50 ? chance_one < 25 ? (int) (std::ceil((1000.f / min_cps) / 50.f)) : (int) (std::floor((1000.f / min_cps) / 50.f)) : chance_two < 25 ? (int)(std::ceil((1000.f / min_cps) / 50.f)) : (int)(std::floor((1000.f / min_cps) / 50.f));
	auto final_tick = chance_final < std::clamp((17 - (int) (min_cps / 2)), 5, 17) ? tick - 1 : chance_final > std::clamp((83 + (int) (min_cps / 2)), 83, 95) ? tick + 1 : tick;

	if (clicks_skip > clicks_until_skip) 
	{
		set_tick_delay(std::clamp(util::random(3, 6), 1, 10));

		skip_click_count++;

		if (skip_click_count >= skip_click_amt)
		{
			clicks_skip = 0;
			skip_click_count = 0;
			clicks_until_skip = util::random(35, 60);
			skip_click_amt = util::random(3, 5);
		}
	}
	else 
	{
		set_tick_delay(std::clamp(final_tick, 1, 10));
	}

	m_left_click = true;
	m_last_click = GetTickCount64();
}

void c_auto_clicker::set_tick_delay(int tick)
{
	current_delay = tick * 50;
}

