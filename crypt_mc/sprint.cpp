#include "context.h"
#include "sprint.h"

void c_sprint::on_get_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (self->get_forward() > 0 || (ctx.m_settings.movement_sprint_omni && (self->motion_x() != 0 || self->motion_z() != 0 || self->get_strafing() > 0)) && !self->is_sneaking())
	{
		self->set_sprinting(true);

		if (ctx.m_settings.movement_sprint_omni && self->get_forward() <= 0.f)
		{
			self->set_motion_x(self->motion_x() * 1.185f);
			self->set_motion_z(self->motion_z() * 1.185f);
		}
	}
}
