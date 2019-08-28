#include "context.h"
#include "sprint.h"

void c_sprint::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.movement_sprint || !valid_keystate(ctx.m_settings.movement_sprint_key))
		return;

	if (self->get_forward() > 0 || (ctx.m_settings.movement_sprint_omni && (self->motion_x() != 0 || self->motion_z() != 0 || self->get_strafing() > 0)))
		self->set_sprinting(true);
}
