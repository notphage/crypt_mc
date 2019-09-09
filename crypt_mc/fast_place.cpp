#include "context.h"
#include "fast_place.h"

void c_fast_place::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.player_fast_place || !valid_keystate(ctx.m_settings.player_fast_place_key))
		return;

	if ((!ctx.m_settings.player_fast_place_blocks && self->holding_block()) || (!ctx.m_settings.player_fast_place_projectiles && self->holding_projectile()))
		return;

	bool projectiles = ctx.m_settings.player_fast_place_projectiles && self->holding_projectile();
	bool blocks = ctx.m_settings.player_fast_place_blocks && self->holding_block();

	int delay = blocks ? ctx.m_settings.player_fast_place_block_delay : projectiles ? ctx.m_settings.player_fast_place_projectiles_delay : 0;

	if (mc->get_right_click_delay() > delay)
		mc->set_right_click_delay(delay);
}
