#include "context.h"
#include "flight.h"

void c_flight::on_get_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (ctx.m_settings.movement_flight_tight)
	{
		self->set_motion_x(0);
		self->set_motion_z(0);
	}

	bool glide = true;

	if (ctx.m_settings.movement_flight_mode != 2)
		if (mc->get_timer_speed() == 0.149f)
			mc->set_timer_speed(1.0f);

	static int count = 0;

	switch (ctx.m_settings.movement_flight_mode)
	{
		case 0:
		{
			self->set_motion_y(0);

			if (self->is_on_ground())
				return;
			count++;

			mc->set_timer_speed(self->get_ticks_existed() % 15 == 0 ? 1.9F : 1.7F);

			if (count == 15)
			{
				self->set_position(self->origin_x(), self->aabb_min_y() + util::random(0.00000000000001235423532523523532521, 0.0000000000000123542353252352353252 * 10), self->origin_z());
				count = 0;
			}

			break;
		}

		case 1:
		{
			self->set_motion_y(-0.005);

			if (self->get_ticks_existed() % 2 == 0)
				self->set_motion_y(0);
			break;
		}

		case 2:
		{
			self->set_motion_y(0);

			mc->set_timer_speed(0.149f);
			break;
		}

		default:
		{
			self->set_motion_y(0);
			break;
		}
	}

	if (g_input.is_key_pressed(KEYS_LEFTSHIFT))
	{
		glide = false;

		self->set_motion_y(-ctx.m_settings.movement_flight_vspeed);
	}

	if (g_input.is_key_pressed(KEYS_SPACE) && ctx.m_settings.movement_flight_mode != 0)
	{
		glide = false;

		self->set_motion_y(ctx.m_settings.movement_flight_vspeed);
	}

	if (g_input.is_key_pressed(KEYS_W) || g_input.is_key_pressed(KEYS_A) || g_input.is_key_pressed(KEYS_S) || g_input.is_key_pressed(KEYS_D))
	{
		set_speed(self, (ctx.m_settings.movement_flight_mode == 0) ? self->get_ticks_existed() % 10 == 0 ? 0.36F : 0.2873F : ctx.m_settings.movement_flight_hspeed);
	}

	if (glide && ctx.m_settings.movement_flight_glide)
	{
		self->set_motion_y(-ctx.m_settings.movement_flight_glide_speed);
	}
}

void c_flight::on_disable(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&)
{
	mc->set_timer_speed(1.f);
}

void c_flight::set_speed(const std::shared_ptr<c_player>& self, float speed) const
{
	self->set_motion_x(-(sin(get_direction(self)) * speed));
	self->set_motion_z(cos(get_direction(self)) * speed);
}

float c_flight::get_direction(const std::shared_ptr<c_player>& self)
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
		forward = 1.f;
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