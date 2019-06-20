#include "context.h"
#include "step.h"

void c_step::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
    if (!ctx.m_settings.movement_step || !valid_keystate(ctx.m_settings.movement_step_key))
    {
        self->set_step_height(0.5f);
        return;
    }

    if (self->is_collided_horizontally() && self->is_collided_vertically()) 
    {
        self->set_step_height(ctx.m_settings.movement_step_height);
    }
    else 
    {
        self->set_step_height(0.5f);
    }
}
