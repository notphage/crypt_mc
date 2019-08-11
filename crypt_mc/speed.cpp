#include "context.h"
#include "speed.h"

void c_speed::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	m_on_ground = self->is_on_ground() && self->is_collided_vertically();

	if (valid_keystate(ctx.m_settings.movement_longjump_key))
		longjump(self);
	else if (ctx.m_settings.movement_speed && valid_keystate(ctx.m_settings.movement_speed_key))
	{
		if (self->get_strafing() != 0 || self->get_forward() != 0)
		{
			switch (ctx.m_settings.movement_speed_mode)
			{
			case 0:
			{
				bhop(self);
				break;
			}

			case 1:
			{
				slowhop(self);
				break;
			}

			case 2:
			{
				minihop(self);
				break;
			}

			case 3:
			{
				yport(self);
				break;
			}

			case 4:
			{
				vanilla(self);
				break;
			}

			case 5:
			{
				glidehop(self);
				break;
			}

			case 6:
			{
				custom(self);
				break;
			}
			default:
				break;
			}
		}
		else
		{
			self->set_motion_x(0);
			self->set_motion_z(0);
		}
	}
	else if (ctx.m_settings.movement_air_control && valid_keystate(ctx.m_settings.movement_air_control_key))
		if (!m_on_ground)
			set_speed(self, get_base_speed(self));
}

void c_speed::bhop(std::shared_ptr<c_player> self) const
{
    if (m_on_ground)
    {
        set_speed(self, get_base_speed(self) + 0.22);

        jump(self, 0.42);
    }
    else 
        set_speed(self, get_base_speed(self) + 0.14);
}

void c_speed::slowhop(std::shared_ptr<c_player> self) const
{
    if (m_on_ground)
    {
        set_speed(self, get_base_speed(self) + 0.14);

        jump(self, 0.42);
    }
    else
        set_speed(self, get_base_speed(self) + 0.03);
}

void c_speed::custom(std::shared_ptr<c_player> self) const
{
    bool jump_on_ground = true;
    bool custom_jump_height = false;
    bool slow_down = false;

    float jump_height = 0.42f;
    float ground_speed = 0.08f;
    float air_speed = 0.14f;
    float slowdown_ticks = 2.f;
    float slowdown_speed = 0.1f;
    float fall_speed = 0.1f;

    if (m_on_ground)
    {
        set_speed(self, get_base_speed(self) + 0.14);

        jump(self, 0.42);
    }
    else
        set_speed(self, get_base_speed(self) + 0.03);
}


void c_speed::minihop(std::shared_ptr<c_player> self) const
{
    if (m_on_ground)
    {
        set_speed(self, get_base_speed(self) + 0.08);

        jump(self, 0.248);
    }
    else 
        set_speed(self, get_base_speed(self) + 0.1);
}

void c_speed::glidehop(std::shared_ptr<c_player> self) const
{
    if (m_on_ground)
    {
        set_speed(self, get_base_speed(self) + 0.6);

        jump(self, 0.42);
    }
    else
    {
        self->set_motion_y(-0.14);

        set_speed(self, get_base_speed(self) + 0.1);
    }
}

void c_speed::yport(std::shared_ptr<c_player> self) const
{
    if (m_on_ground)
    {
        set_speed(self, get_base_speed(self) + 0.33);

        jump(self, 0.42);
    }
    else 
    {
        set_speed(self, get_base_speed(self) + 0.5);

        self->set_motion_y(-10);
    }
}

void c_speed::vanilla(std::shared_ptr<c_player> self) const
{
    if (m_on_ground)
        set_speed(self, get_base_speed(self) + 0.15);
}

void c_speed::longjump(std::shared_ptr<c_player> self) const
{
	if (m_on_ground)
	{
		set_speed(self, get_base_speed(self) + ctx.m_settings.movement_longjump_boost);
		jump(self, 0.42);
	}
}

void c_speed::jump(std::shared_ptr<c_player> self, double height) const
{
	(height == 0.42) ? self->set_motion_y(0.41999998688697815) : self->set_motion_y(height);
}

double c_speed::get_base_speed(std::shared_ptr<c_player> self)
{
    double base_speed = 0.283;

    //Handle speed potion
    if (self->is_potion_active(1))
    {
        int amplifier = self->get_speed_amplifier();

        base_speed *= 1.0 + 0.2 * (amplifier + 1);
    }

    return base_speed;
}

void c_speed::set_speed(std::shared_ptr<c_player> self, float speed) const
{
	self->set_motion_x(-(sin(get_direction(self)) * speed));
	self->set_motion_z(cos(get_direction(self)) * speed);
}

//Intense fucking math
float c_speed::get_direction(std::shared_ptr<c_player> self)
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