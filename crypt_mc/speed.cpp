#include "context.h"
#include "speed.h"

void c_base_speed::jump(const std::shared_ptr<c_player>& self, float height)
{
	self->set_motion_y(height);
}

float c_base_speed::get_base_speed(const std::shared_ptr<c_player>& self)
{
	return self->is_potion_active(1) ? 0.2873f * 1.f + 0.2f * static_cast<float>((self->get_speed_amplifier()) + 1) : 0.2873f;
}

void c_base_speed::set_speed(const std::shared_ptr<c_player>& self, float speed) const
{
	self->set_motion_x(-(sin(get_direction(self)) * speed));
	self->set_motion_z(cos(get_direction(self)) * speed);
}

//Intense fucking math
float c_base_speed::get_direction(const std::shared_ptr<c_player>& self)
{
	auto yaw = self->get_yaw();

	const auto moving_forward = self->get_forward();
	const auto moving_strafing = self->get_strafing();

	float forward;

	if (moving_forward < 0.0F)
		yaw += 180;

	if (moving_forward < 0)
		forward = -0.5f;
	else if (moving_forward > 0)
		forward = 0.5f;
	else
		forward = 1;

	if (moving_strafing > 0.0F)
		yaw -= 90.0F * forward;

	if (moving_strafing < 0.0F)
		yaw += 90.0F * forward;

	yaw *= 0.017453292F;

	return yaw;
}

void c_speed::bhop(const std::shared_ptr<c_player>& self) const
{
	set_speed(self, get_base_speed(self) + (m_on_ground ? 0.22f : 0.14f));

	if (m_on_ground)
		jump(self, 0.42f);
}

void c_speed::slowhop(const std::shared_ptr<c_player>& self) const
{
	set_speed(self, get_base_speed(self) + (m_on_ground ? 0.17f : 0.05f));

	if (m_on_ground)
		jump(self, 0.42f);
}

void c_speed::custom(const std::shared_ptr<c_player>& self) const
{
	bool slow = ctx.m_settings.movement_speed_custom_slow && self->get_ticks_existed() % ctx.m_settings.movement_speed_custom_slow_down_ticks == 0;

	if (m_on_ground && ctx.m_settings.movement_speed_custom_jump_ground)
		jump(self, ctx.m_settings.movement_speed_custom_jump_height ? ctx.m_settings.movement_speed_custom_jump_height_val : 0.42f);

	if (!m_on_ground && ctx.m_settings.movement_speed_custom_fall)
		self->set_motion_y(-ctx.m_settings.movement_speed_custom_fall_speed_val);

	if (ctx.m_settings.movement_speed_custom_slow_down && slow)
		set_speed(self, ctx.m_settings.movement_speed_custom_slow_speed_val);
	else if (slow)
		set_speed(self, get_base_speed(self) - 0.05f);
	else
		set_speed(self, (m_on_ground ? ctx.m_settings.movement_speed_custom_ground_speed_val : ctx.m_settings.movement_speed_custom_air_speed_val));
}

void c_speed::minihop(const std::shared_ptr<c_player>& self) const
{
	set_speed(self, get_base_speed(self) + (m_on_ground ? 0.14f : 0.1f));

	if (m_on_ground)
		jump(self, 0.248f);
}

void c_speed::glidehop(const std::shared_ptr<c_player>& self) const
{
	set_speed(self, get_base_speed(self) + (m_on_ground ? 0.6f : 0.1f));

	if (m_on_ground)
		jump(self, 0.42f);
	else
		self->set_motion_y(-0.14f);
}

void c_speed::yport(const std::shared_ptr<c_player>& self) const
{
	set_speed(self, get_base_speed(self) + (m_on_ground ? 0.33f : 0.5f));

	if (m_on_ground)
		jump(self, 0.42f);
	else
		self->set_motion_y(-10.f);
}

void c_speed::vanilla(const std::shared_ptr<c_player>& self) const
{
	if (m_on_ground)
		set_speed(self, get_base_speed(self) + 0.15f);
}

void c_speed::on_get_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	m_on_ground = self->is_on_ground() && self->is_collided_vertically();

	if (self->get_strafing() != 0.f || self->get_forward() != 0.f)
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

void c_air_control::on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	m_on_ground = self->is_on_ground() && self->is_collided_vertically();

	if (!m_on_ground)
		set_speed(self, (get_base_speed(self) - 0.2873f) + ctx.m_settings.movement_air_control_speed);
}

void c_long_jump::on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	m_on_ground = self->is_on_ground() && self->is_collided_vertically();

	if (m_on_ground)
	{
		set_speed(self, get_base_speed(self) + ctx.m_settings.movement_longjump_boost);

		jump(self, 0.42f);
	}
}
