#include "context.h"
#include "timer.h"

void c_timer::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
    if (!ctx.m_settings.movement_timer || !valid_keystate(ctx.m_settings.movement_timer_key))
    {
        mc->set_timer_speed(1.f);
        return;
    }

	mc->set_timer_speed(ctx.m_settings.movement_timer_speed);
}
