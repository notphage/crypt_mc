#include "context.h"
#include "step.h"

void c_step::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
    if (self->is_collided_horizontally() && self->is_collided_vertically()) 
        self->set_step_height(ctx.m_settings.movement_step_height);
	else
		self->set_step_height(.5f);
}

void c_step::on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	self->set_step_height(0.5f);
}
