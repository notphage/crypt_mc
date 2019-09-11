#include "context.h"
#include "velocity.h"

void c_velocity::on_atan2(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	static bool should_run = true;

	if (ctx.m_settings.combat_velocity_on_sprint && !self->is_sprinting())
		return;

	const int chance = util::random(0, 100);
	const auto hurt_time = self->get_hurt_time();
	const auto max_hurt_time = self->get_max_hurt_time();
	const auto delay_amount = ctx.m_settings.combat_velocity_delay ? max_hurt_time - ctx.m_settings.combat_velocity_delay_ticks : max_hurt_time;
	static auto last_hurt_time = hurt_time;
	bool should_run_nine = last_hurt_time != max_hurt_time && last_hurt_time != max_hurt_time - 1 && hurt_time == 9 && should_run;

	if ((hurt_time == delay_amount || should_run_nine) && hurt_time != 0 && chance <= ctx.m_settings.combat_velocity_chance && should_run)
	{
		should_run = false;

		//Vertical
		self->set_motion_y(self->motion_y() * ((float)ctx.m_settings.combat_velocity_vertical / 100.f));

		//Horizontal
		self->set_motion_x(self->motion_x() * ((float)ctx.m_settings.combat_velocity_horizontal / 100.f));
		self->set_motion_z(self->motion_z() * ((float)ctx.m_settings.combat_velocity_horizontal / 100.f));
	}
	else if (hurt_time < delay_amount)
		should_run = true;

	last_hurt_time = hurt_time;
}
