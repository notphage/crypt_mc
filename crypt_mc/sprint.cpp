#include "context.h"
#include "sprint.h"

void c_sprint::on_get_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	self->set_sprinting(self->get_forward() > 0.f && !self->is_sneaking() && !self->is_collided_horizontally());
}
