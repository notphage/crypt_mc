#include "context.h"
#include "auto_clicker.h"
#include <random>

bool left_click = true;

int current_delay = 0;
int clicks = 0;
int clicks_until_skip = 38;

long last_click = 0;

void c_auto_clicker::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_auto_clicker || !valid_keystate(ctx.m_settings.combat_auto_clicker_key) || ctx.m_menu_open)
		return;

	bool in_inventory = mc->is_in_inventory();
	bool has_weapon = self->holding_weapon();
	bool has_focus = mc->in_game_has_focus();

	if (!in_inventory && !has_focus)
		return;

	if (!ctx.m_settings.combat_auto_clicker_inventory && in_inventory)
		return;


	if (!in_inventory && ctx.m_settings.combat_auto_clicker_weapons_only && !has_weapon)
		return;

	if (in_inventory && ctx.m_settings.combat_auto_clicker_inventory && !g_input.is_key_pressed(KEYS_SHIFT))
		return;

	auto mouse_down = GetAsyncKeyState(VK_LBUTTON) < 0;

	if (mouse_down)
	{
		if (last_click == 0)
			last_click = clock();

		if (clicks > clicks_until_skip)
		{
			if (clock() - last_click > 80)
			{
				left_click = true;
				current_delay = std::clamp(util::random_delay(ctx.m_settings.combat_auto_clicker_min_cps, ctx.m_settings.combat_auto_clicker_max_cps + 1) + util::random(-10, 10), 10, 2000);
				last_click = clock();
				clicks = 0;
				clicks_until_skip = util::random(43, 62);
			}
			return;
		}

		if (left_click && ((clock() - last_click) > util::random(current_delay / 2, (current_delay / 2) + current_delay / util::random(9, 21))))
		{
			g_input.press_mouse(true);
			left_click = false;
		}
		else if (!left_click && clock() - last_click > current_delay)
		{
			g_input.release_mouse(true);

			left_click = true;
			current_delay = std::clamp(util::random_delay(ctx.m_settings.combat_auto_clicker_min_cps, ctx.m_settings.combat_auto_clicker_max_cps + 1) + util::random(-10, 10), 10, 2000);
			last_click = clock();
			clicks += 1;
		}
		return;
	}

	last_click = 0;
}