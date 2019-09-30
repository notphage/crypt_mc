#include "context.h"
#include "auto_clicker.h"
#include <random>

void c_auto_clicker::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
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

		if (m_clicks_change > m_clicks_until_change)
		{
			if (!m_set_change)
			{
				const float random = util::random(0.95f, 1.155f);
		
				m_min_cps = abs(ctx.m_settings.combat_auto_clicker_min_cps * random);
				m_max_cps = abs(ctx.m_settings.combat_auto_clicker_max_cps * random);
		
				m_set_change = true;
			}
		
			if (m_clicks_change - m_clicks_until_change > m_clicks_until_escape)
			{
				m_clicks_change = 0;
				m_clicks_until_change = util::random(35, 60);
				m_clicks_until_escape = util::random(4, 9);
			}
		}
		else
		{
			m_min_cps = ctx.m_settings.combat_auto_clicker_min_cps;
			m_max_cps = ctx.m_settings.combat_auto_clicker_max_cps + 1;
		
			m_set_change = false;
		}

		if (m_clicks_skip > m_clicks_until_skip)
		{
			if (GetTickCount64() - m_last_click > 80)
			{
				m_skip_click_count++;

				if (m_skip_click_count >= m_skip_click_amt)
				{
					m_clicks_skip = 0;
					m_skip_click_count = 0;
					m_clicks_until_skip = util::random(57, 70) + ctx.m_settings.combat_auto_clicker_max_cps;
					m_skip_click_amt = util::random(2, 4);
				}

				reset();
			}
			return;
		}

		if (right_mouse_down && ctx.m_settings.combat_auto_clicker_block_hit)
		{
			if (((GetTickCount64() - m_last_click) <= m_current_delay * 0.1f && (GetTickCount64() - m_last_click) >= m_current_delay * 0.025f) || (GetTickCount64() - m_last_click <= m_current_delay * 0.75 && GetTickCount64() - m_last_click > m_current_delay * 0.7))
			{
				if (!m_blocking)
				{
					m_blocking = true;

					g_input.press_mouse(false);
				}
			}
			else if (m_blocking)
			{
				m_blocking = false;

				g_input.release_mouse(false);
			}
		}

		
		if (m_left_click && ((GetTickCount64() - m_last_click) > util::random(static_cast<int>(m_current_delay * m_multiplier), static_cast<int>(m_current_delay * m_multiplier))))
		{
		
			if (m_blocking)
				g_input.release_mouse(false);

			g_input.press_mouse(true);
			m_left_click = false;
			m_multiplier = util::random(0.35, 0.65);
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
	if (m_clicks < 20)
		m_current_delay = std::clamp(util::random_delay(m_min_cps + 1, m_max_cps + 2), 10, 2000);
	else if (m_clicks < 40)
		m_current_delay = std::clamp(util::random_delay(m_min_cps + 2, m_max_cps + 2), 10, 2000);
	else if (m_clicks < 60)
		m_current_delay = std::clamp(util::random_delay(m_min_cps -1, m_max_cps + 1), 10, 2000);
	else if (m_clicks < 80)
		m_current_delay = std::clamp(util::random_delay(m_min_cps - 1, m_max_cps + 2), 10, 2000);
	else if (m_clicks <= 100)
	{
		m_current_delay = std::clamp(util::random_delay(m_min_cps, m_max_cps), 10, 2000);

		if (m_clicks == 100)
			m_clicks = 0;
	}

	if (util::random(1, 100) < 30) 
		m_current_delay *= util::random(0.9f, 1.1f);
	else 
		m_current_delay = m_current_delay + util::random(-15, 20);

	m_left_click = true;
	m_last_click = GetTickCount64();
}

