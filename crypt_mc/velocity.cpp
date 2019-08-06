#include "context.h"
#include "velocity.h"

void c_velocity::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	static bool should_run = true;

	if (!ctx.m_settings.combat_velocity || !valid_keystate(ctx.m_settings.combat_velocity_key))
		return;

	if (ctx.m_settings.combat_velocity_on_sprint && !self->is_sprinting())
		return;

	int chance = util::random(0, 100);
	auto delay_amount = ctx.m_settings.combat_velocity_delay ? self->get_max_hurt_time() - ctx.m_settings.combat_velocity_delay_ticks - 1 : 10;

	if (self->get_hurt_time() == delay_amount && self->get_hurt_time() != 0 && chance <= ctx.m_settings.combat_velocity_chance && should_run)
	{
		should_run = false;

		//Vertical
		self->set_motion_y(self->motion_y() * ((float)ctx.m_settings.combat_velocity_vertical / 100.f));

		//Horizontal
		self->set_motion_x(self->motion_x() * ((float)ctx.m_settings.combat_velocity_horizontal / 100.f));
		self->set_motion_z(self->motion_z() * ((float)ctx.m_settings.combat_velocity_horizontal / 100.f));
	}
	else if (self->get_hurt_time() < delay_amount)
		should_run = true;
}
