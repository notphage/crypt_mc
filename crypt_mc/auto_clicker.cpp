#include "context.h"
#include "auto_clicker.h"
#include <random>

bool left_click = true, set_change = false;

int current_delay = 0;

int min_cps = ctx.m_settings.combat_auto_clicker_min_cps, max_cps = ctx.m_settings.combat_auto_clicker_max_cps;
int clicks_skip = 0, clicks_until_skip = 38;
int clicks_change = 0, clicks_until_change = 50, clicks_until_escape = 12;

float multiplier = 0.5;

long last_click = 0;

void c_auto_clicker::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_auto_clicker || !valid_keystate(ctx.m_settings.combat_auto_clicker_key) || ctx.m_menu_open)
		return;

	const bool in_inventory = mc->is_in_inventory();
	const bool has_weapon = self->holding_weapon();
	//const bool has_weapon = false;
	const bool has_focus = mc->in_game_has_focus();

	if (!in_inventory && !has_focus)
		return;

	if (!ctx.m_settings.combat_auto_clicker_inventory && in_inventory)
		return;

	if (!in_inventory && ctx.m_settings.combat_auto_clicker_weapons_only && !has_weapon)
		return;

	if (in_inventory && ctx.m_settings.combat_auto_clicker_inventory && !g_input.is_key_pressed(KEYS_SHIFT))
		return;

	const auto mouse_down = GetAsyncKeyState(VK_LBUTTON) < 0;

	if (mouse_down)
	{
		if (last_click == 0)
			last_click = clock();

		if (clicks_change > clicks_until_change)
		{
			if (!set_change)
			{
				const float random = util::random(1.6f, 1.8f);

				const float rand_multiplier = util::random(0, 100) > 80 ? -1.f : 1.f;

				min_cps = ctx.m_settings.combat_auto_clicker_min_cps * static_cast<int>(random * rand_multiplier);
				max_cps = ctx.m_settings.combat_auto_clicker_max_cps * static_cast<int>(random * rand_multiplier);

				set_change = true;
			}

			if (clicks_change - clicks_until_change > clicks_until_escape)
			{
				clicks_change = 0;
				clicks_until_change = util::random(40, 65);
				clicks_until_escape = util::random(8, 14);
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
			if (clock() - last_click > 80)
			{
				left_click = true;
				current_delay = std::clamp(util::random_delay(ctx.m_settings.combat_auto_clicker_min_cps, ctx.m_settings.combat_auto_clicker_max_cps + 1) + util::random(-10, 10), 10, 2000);
				last_click = clock();
				clicks_skip = 0;
				clicks_until_skip = util::random(43, 62);
			}
			return;
		}

		if (left_click && ((clock() - last_click) > util::random(static_cast<int>(current_delay * multiplier), static_cast<int>(current_delay * multiplier) + current_delay / util::random(9, 21))))
		{
			g_input.press_mouse(true);
			left_click = false;
			multiplier = util::random(0.45, 0.55);
		}
		else if (!left_click && clock() - last_click > current_delay)
		{
			g_input.release_mouse(true);

			left_click = true;
			current_delay = std::clamp(util::random_delay(ctx.m_settings.combat_auto_clicker_min_cps, ctx.m_settings.combat_auto_clicker_max_cps + 1) + util::random(-10, 10), 10, 2000);
			last_click = clock();
			clicks_skip += 1;
			clicks_change += 1;
		}
		return;
	}

	last_click = 0;
}