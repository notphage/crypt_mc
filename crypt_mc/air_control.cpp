#include "context.h"
#include "air_control.h"

void c_air_control::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
    if (!ctx.m_settings.movement_air_control || !valid_keystate(ctx.m_settings.movement_air_control_key))
        return;

    if (!self->is_collided_vertically())
	{
        set_speed(self, get_base_speed(self));
    }
}

void c_air_control::set_speed(std::shared_ptr<c_player> self, float speed) const
{
    self->set_motion_x(-(sin(get_direction(self)) * speed));
    self->set_motion_z(cos(get_direction(self)) * speed);
}

//Intense fucking math
float c_air_control::get_direction(std::shared_ptr<c_player> self)
{
    auto yaw = self->get_yaw();

    auto moving_forward = self->get_forward();
    auto moving_strafing = self->get_strafing();

    float forward;

    if (moving_forward < 0.0F)
    {
        yaw += 180;
    }

    if (moving_forward < 0)
    {
        forward = -0.5f;
    }
    else if (moving_forward > 0)
    {
        forward = 0.5f;
    }
    else
    {
        forward = 1;
    }

    if (moving_strafing > 0.0F)
    {
        yaw -= 90.0F * forward;
    }

    if (moving_strafing < 0.0F)
    {
        yaw += 90.0F * forward;
    }

    yaw *= 0.017453292F;

    return yaw;
}

double c_air_control::get_base_speed(std::shared_ptr<c_player> self)
{
    double base_speed = 0.2873;

    //Handle speed potion
    if (self->is_potion_active(1))
    {
        int amplifier = self->get_speed_amplifier();

        base_speed *= 1.0 + 0.2 * (amplifier + 1);
    }

    return base_speed;
}
