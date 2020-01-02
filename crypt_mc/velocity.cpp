#include "context.h"
#include "velocity.h"

void c_velocity::on_atan2(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	static bool should_run = true;

	if (ctx.m_settings.combat_velocity_on_sprint && !self->is_sprinting())
		return;

	if (ctx.m_settings.combat_velocity_require_click && !GetAsyncKeyState(VK_LBUTTON) < 0)
		return;

	const jint hurt_time = self->get_hurt_time(), max_hurt_time = self->get_max_hurt_time(), chance = util::random(0, 100);
	auto last_hurt_time = hurt_time;
	const auto delay_amount = ctx.m_settings.combat_velocity_delay ? max_hurt_time - ctx.m_settings.combat_velocity_delay_ticks : max_hurt_time;
	auto should_run_nine = last_hurt_time != max_hurt_time && last_hurt_time != max_hurt_time - 1 && hurt_time == 9 && should_run;

	if ((hurt_time == delay_amount || should_run_nine) && hurt_time != 0 && chance <= ctx.m_settings.combat_velocity_chance && should_run)
	{
		should_run = false;

		const auto player_relative = check_player_pos(mc, self, world);
		const bool should_kite = ctx.m_settings.combat_velocity_kite && !player_relative.first && player_relative.second;

		if (ctx.m_settings.combat_velocity_require_target && !player_relative.first && !should_kite)
			return;

		auto horizontal = should_kite ? ctx.m_settings.combat_velocity_kite_val : ctx.m_settings.combat_velocity_horizontal;
		auto vertical = should_kite ? 100 : ctx.m_settings.combat_velocity_vertical;

		//Vertical
		self->set_motion_y(self->motion_y() * ((float)vertical / 100.f));

		//Horizontal
		self->set_motion_x(self->motion_x() * ((float)horizontal / 100.f));
		self->set_motion_z(self->motion_z() * ((float)horizontal / 100.f));
	}
	else if (hurt_time < delay_amount)
		should_run = true;

	last_hurt_time = hurt_time;
}

std::pair<bool, bool> c_velocity::check_player_pos(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	bool player_infront = false;
	bool player_behind = false;

	const vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());
	const vec3 self_prev_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
	vec3 trace_origin = self_prev_origin + ((self_origin - self_prev_origin) * mc->get_render_partial_ticks());
	trace_origin.y += self->get_eye_height();

	const vec3 self_forward_angle(self->get_pitch(), self->get_yaw(), 0.f);
	vec3 self_forward;
	math::angle_vectors(self_forward_angle, &self_forward);

	const vec3 self_backward_angle(self->get_pitch(), self->get_yaw() - 180.f, 0.f);
	vec3 self_backward;
	math::angle_vectors(self_backward_angle, &self_backward);

	for (const auto& player : world->get_players())
	{
		if (!player || !player->player_instance || player->is_dead() || self->is_same(player) || !self->is_visible(player->player_instance))
			continue;

		if (player_behind && player_infront)
			break;

		const vec3 player_origin(player->origin_x(), player->origin_y(), player->origin_z());

		if ((self_origin - player_origin).length() > 8.0f)
			continue;

		vec3 forward_player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
		vec3 forward_player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());
		vec3 behind_player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
		vec3 behind_player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());

		forward_player_mins -= player_origin;
		forward_player_maxs -= player_origin;

		forward_player_mins *= 2.f;
		forward_player_maxs *= 2.f;
		forward_player_maxs.y = std::copysignf(1.5f, forward_player_maxs.y) + (.3f * 2.f);

		forward_player_mins += player_origin;
		forward_player_maxs += player_origin;


		behind_player_mins -= player_origin;
		behind_player_maxs -= player_origin;

		behind_player_mins *= 3.f;
		behind_player_maxs *= 3.f;
		behind_player_maxs.y = std::copysignf(1.5f, behind_player_maxs.y) + (.3f * 2.f);

		behind_player_mins += player_origin;
		behind_player_maxs += player_origin;

		if (!player_infront)
		{
			ray_trace_t forward_ray_trace(trace_origin, self_forward);

			float foward_distance = 0.f;
			vec3 hit_position = {};
			if (forward_ray_trace.trace(forward_player_mins, forward_player_maxs, foward_distance, hit_position))
			{
				if (foward_distance < 6.0f)
				{
					player_infront = true;
				}
			}
		}

		if (!player_behind)
		{
			ray_trace_t backward_ray_trace(trace_origin, self_backward);

			float backward_distance = 0.f;
			vec3 hit_position = {};
			if (backward_ray_trace.trace(behind_player_mins, behind_player_maxs, backward_distance, hit_position))
			{
				if (backward_distance < 6.0f)
					player_behind = true;
			}
		}
	}

	return { player_infront, player_behind };
}
