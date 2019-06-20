#include "context.h"
#include "fast_place.h"

void c_fast_place::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.player_fast_place || !valid_keystate(ctx.m_settings.player_fast_place_key))
		return;

	if (mc->get_right_click_delay() != ctx.m_settings.player_fast_place_delay)
		mc->set_right_click_delay(ctx.m_settings.player_fast_place_delay);
}
