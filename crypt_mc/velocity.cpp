#include "context.h"
#include "velocity.h"

void c_velocity::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_velocity || !valid_keystate(ctx.m_settings.combat_velocity_key))
		return;

	if (ctx.m_settings.combat_velocity_on_sprint && !self->is_sprinting())
		return;

	int chance = util::random(0, 100);

	if (self->get_hurt_time() + 1 == (self->get_max_hurt_time() - ctx.m_settings.combat_velocity_delay) && self->get_hurt_time() != 0 && chance <= ctx.m_settings.combat_velocity_chance)
	{
		//Vertical
		self->set_motion_y(self->motion_y() * (ctx.m_settings.combat_velocity_vertical / 100.0f));

		//Horizontal
		self->set_motion_x(self->motion_x() * (ctx.m_settings.combat_velocity_horizontal / 100.0f));
		self->set_motion_z(self->motion_z() * (ctx.m_settings.combat_velocity_horizontal / 100.0f));
	}
}
