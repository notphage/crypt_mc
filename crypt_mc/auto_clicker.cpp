#include "context.h"
#include "auto_clicker.h"
#include <random>

bool left_click = true, set_change = false;

int current_delay = 0;

int min_cps = ctx.m_settings.combat_auto_clicker_min_cps, max_cps = ctx.m_settings.combat_auto_clicker_max_cps;
int clicks_skip = 0, clicks_until_skip = 38;
int clicks_change = 0, clicks_until_change = 50, clicks_until_escape = 12;

float multiplier = 0.5;

uint64_t last_click = 0;

void c_auto_clicker::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_auto_clicker || !valid_keystate(ctx.m_settings.combat_auto_clicker_key) || ctx.m_menu_open)
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
		if (ctx.m_settings.combat_auto_clicker_axes && self->holding_axe())
			white_list = true;

		if (ctx.m_settings.combat_auto_clicker_swords && self->holding_sword())
			white_list = true;

		if (ctx.m_settings.combat_auto_clicker_hoes && self->holding_hoe())
			white_list = true;

		if (ctx.m_settings.combat_auto_clicker_shovels && self->holding_shovel())
			white_list = true;

		if (ctx.m_settings.combat_auto_clicker_pick_axes && self->holding_pick_axe())
			white_list = true;

		if (!white_list)
			return;
	}

	if (in_inventory && ctx.m_settings.combat_auto_clicker_inventory && !g_input.is_key_pressed(KEYS_SHIFT))
		return;

	const auto mouse_down = GetAsyncKeyState(VK_LBUTTON) < 0;

	static bool init_block_break = false;

	if (ctx.m_settings.combat_auto_clicker_break_blocks && mc->is_hovering_block() && !mc->is_in_inventory())
	{
		left_click = true;
		current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-20, 20), 10, 2000);
		last_click = GetTickCount64();
		clicks_skip = 0;
		clicks_until_skip = util::random(43, 62);

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
	{
		init_block_break = false;
	}

	if (mouse_down)
	{
		if (last_click == 0)
			last_click = GetTickCount64();

		if (clicks_change > clicks_until_change)
		{
			if (!set_change)
			{
				const float random = util::random(1.4f, 1.5f);

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
			if (GetTickCount64() - last_click > 80)
			{
				left_click = true;
				current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-20, 20), 10, 2000);
				last_click = GetTickCount64();
				clicks_skip = 0;
				clicks_until_skip = util::random(43, 62);
			}
			return;
		}

		if (left_click && ((GetTickCount64() - last_click) > util::random(static_cast<int>(current_delay * multiplier), static_cast<int>(current_delay * multiplier) + current_delay / util::random(9, 21))))
		{
			g_input.press_mouse(true);
			left_click = false;
			multiplier = util::random(0.4, 0.575);
		}
		else if (!left_click && GetTickCount64() - last_click > current_delay)
		{
			g_input.release_mouse(true);

			left_click = true;
			current_delay = std::clamp(util::random_delay(min_cps, max_cps) + util::random(-20, 20), 10, 2000);
			last_click = GetTickCount64();
			clicks_skip += 1;
			clicks_change += 1;
		}
		return;
	}

	last_click = 0;
}