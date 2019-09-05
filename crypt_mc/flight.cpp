#include "context.h"
#include "flight.h"

void c_flight::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
    if (!ctx.m_settings.movement_flight || !valid_keystate(ctx.m_settings.movement_flight_key))
        return;

    if (ctx.m_settings.movement_flight_tight)
	{
        self->set_motion_x(0);
        self->set_motion_z(0);
    }

    self->set_motion_y(0);

    bool glide = true;

	if (self->get_ticks_existed() % 2 == 0 && ctx.m_settings.movement_flight_hypixel)
		self->set_position(self->origin_x(), self->aabb_min_y() + util::random(0.00000000000001235423532523523532521, 0.0000000000000123542353252352353252 * 10), self->origin_z());

    if (g_input.is_key_pressed(KEYS_LEFTSHIFT)) 
    {
        glide = false;

        self->set_motion_y(-ctx.m_settings.movement_flight_vspeed);
    }

    if (g_input.is_key_pressed(KEYS_SPACE) && !ctx.m_settings.movement_flight_hypixel)
    {
        glide = false;

        self->set_motion_y(ctx.m_settings.movement_flight_vspeed);
    }

    if (g_input.is_key_pressed(KEYS_W) || g_input.is_key_pressed(KEYS_A) || g_input.is_key_pressed(KEYS_S) || g_input.is_key_pressed(KEYS_D))
    {
        set_speed(self, ctx.m_settings.movement_flight_hypixel ? 0.2873F : ctx.m_settings.movement_flight_hspeed);
    }

    if (glide && ctx.m_settings.movement_flight_glide)
	{
        self->set_motion_y(-ctx.m_settings.movement_flight_glide_speed);
    }
}

void c_flight::set_speed(std::shared_ptr<c_player> self, float speed) const
{
    self->set_motion_x(-(sin(get_direction(self)) * speed));
    self->set_motion_z(cos(get_direction(self)) * speed);
}

float c_flight::get_direction(std::shared_ptr<c_player> self)
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