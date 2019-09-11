#include "context.h"
#include "timer.h"

void c_timer::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&)
{
	mc->set_timer_speed(ctx.m_settings.movement_timer_speed);
}

void c_timer::on_disable(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&)
{
	mc->set_timer_speed(1.f);
}
