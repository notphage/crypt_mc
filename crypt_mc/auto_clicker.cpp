#include "context.h" 
#include "auto_clicker.h" 
#include <random> 

bool m_left_click = true, m_right_click = true;
bool m_set_change = false;
bool m_blocking = false;

uint32_t m_clicks_skip = 0;
uint32_t m_clicks_until_skip = 57;
uint32_t m_skip_click_amt = 2;
uint32_t m_skip_click_count = 0;
uint32_t m_clicks_change = 0;
uint32_t m_clicks_until_change = 50;
uint32_t m_clicks_until_escape = 9;
uint32_t m_current_delay = 0;
uint32_t m_current_r_delay = 0;
uint32_t m_clicks = 0;
uint32_t m_last_click = 0;
uint32_t m_last_r_click = 0;

float m_multiplier = 0.5f;

long last_attack = GetTickCount64();

bool auto_block = false;

void c_auto_clicker::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (ctx.m_menu_open)
	{
		reset();
		return;
	}

	if (m_current_delay == 0 || m_last_click == 0)
		reset();

	const bool in_inventory = mc->is_in_inventory();
	const bool has_focus = mc->in_game_has_focus();

	if (!in_inventory && !has_focus)
	{
		reset();
		return;
	}


	if (!ctx.m_settings.combat_auto_clicker_inventory && in_inventory)
	{
		reset();
		return;
	}

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
		{
			reset();
			return;
		}
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


		if (ctx.m_settings.combat_auto_clicker_auto_block && auto_block)
		{
			if (m_right_click)
			{
				g_input.press_mouse(false);


				m_current_r_delay = 50 * util::random(1, 2);
				m_last_r_click = GetTickCount64();
				m_right_click = false;
			}
			else if (GetTickCount64() - m_last_r_click > m_current_r_delay)
			{

				g_input.release_mouse(false);

				auto_block = false;
				m_right_click = true;
			}

		}
		else
		{
			if (m_left_click && ((GetTickCount64() - m_last_click) > m_current_delay * m_multiplier))
			{
				g_input.press_mouse(true);
				m_left_click = false;
				m_multiplier = util::random(0.3, 0.7);

				if (GetTickCount64() - last_attack >= ctx.m_settings.combat_auto_clicker_block_delay && ctx.m_settings.combat_auto_clicker_auto_block)
				{
					if (mc->get_mouse_over() != nullptr && mc->get_pointed_entity() != nullptr)
					{
						last_attack = GetTickCount64();

						auto_block = true;
					}
				}
			}
			else if (!m_left_click && GetTickCount64() - m_last_click > m_current_delay)
			{
				g_input.release_mouse(true);

				m_clicks += 1;

				reset();

				m_clicks_skip += 1;
				m_clicks_change += 1;
			}
		}

		return;
	}

	m_last_click = GetTickCount64();
}

int random_reset = 0;

void c_auto_clicker::reset()
{
	if (random_reset == 0)
		random_reset = util::random(800, 1500);

	if (m_clicks < 100)
	{
		set_delay(util::random(25, 35), util::random(45, 75));
	}
	else if (m_clicks < 300)
	{
		set_delay(util::random(25, 45), util::random(45, 60));
	}
	else if (m_clicks < 500)
	{
		set_delay(util::random(35, 55), util::random(50, 55));
	}
	else if (m_clicks < 750)
	{
		set_delay(util::random(25, 60), util::random(63, 66));
	}
	else if (m_clicks < 1000)
	{
		set_delay(util::random(50, 70), util::random(73, 76));
	}
	else if (m_clicks < 1150)
	{
		set_delay(util::random(30, 50), util::random(50, 60));
	}
	else if (m_clicks < 1300)
	{
		set_delay(util::random(30, 45), util::random(55, 60));
	}
	else if (m_clicks <= 1500)
	{
		set_delay(util::random(25, 35), util::random(35, 45));
	}

	if (m_clicks >= random_reset)
	{
		random_reset = util::random(800, 1500);
		m_clicks = 0;
	}

	m_left_click = true;
	m_last_click = GetTickCount64();
}

void c_auto_clicker::set_delay(int slow_chance, int fast_chance)
{
	auto random = util::random(0, 50) + util::random(0, 50);
	auto base_random = util::random(0, 100);

	if (m_clicks_skip > m_clicks_until_skip)
	{
		m_current_delay = util::random(200, 300);

		m_skip_click_count++;

		if (m_skip_click_count >= m_skip_click_amt)
		{
			m_clicks_skip = 0;
			m_skip_click_count = 0;
			m_clicks_until_skip = util::random(30, 50);
			m_skip_click_amt = util::random(1, 2);
		}

		return;
	}

	if (random <= slow_chance)
	{
		m_current_delay = util::random(1100.f + (base_random / 2), 1300.f + base_random) / ctx.m_settings.combat_auto_clicker_cps;
	}
	else if (random <= fast_chance)
	{
		m_current_delay = util::random(650.0f + base_random, 850.f + base_random) / ctx.m_settings.combat_auto_clicker_cps;
	}
	else
	{
		m_current_delay = util::random(900.f - base_random, 1100.f + base_random) / ctx.m_settings.combat_auto_clicker_cps;
	}
}