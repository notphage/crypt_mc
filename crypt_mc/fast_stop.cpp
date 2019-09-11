#include "context.h"

#include "fast_stop.h"

void c_fast_stop::on_get_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (self->get_strafing() == 0.f && self->get_forward() == 0.f && self->get_hurt_time() == 0)
	{
		self->set_motion_x(0);
		self->set_motion_z(0);
	}
}
