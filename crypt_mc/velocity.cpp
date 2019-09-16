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

		bool is_player_in_front = is_player_infront(mc, self, world);
		bool should_kite = ctx.m_settings.combat_velocity_kite && !is_player_in_front && is_player_behind(mc, self, world);

		if (ctx.m_settings.combat_velocity_require_target && !is_player_in_front && !should_kite)
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

bool c_velocity::is_player_infront(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	const vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());
	const vec3 self_prev_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
	vec3 trace_origin = self_prev_origin + ((self_origin - self_prev_origin) * mc->get_render_partial_ticks());
	trace_origin.y += self->get_eye_height();

	const vec3 self_angle(self->get_pitch(), self->get_yaw(), 0.f);
	vec3 self_forward;
	math::angle_vectors(self_angle, &self_forward);

	for (const auto& player : world->get_players())
	{
		if (!player || !player->player_instance || player->is_dead() || self->is_same(player) || !self->is_visible(player->player_instance))
			continue;

		const vec3 player_origin(player->origin_x(), player->origin_y(), player->origin_z());

		if ((self_origin - player_origin).length() > 8.0f)
			continue;

		vec3 player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
		vec3 player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());

		player_mins -= player_origin;
		player_maxs -= player_origin;

		player_mins *= 2;
		player_maxs *= 2;
		player_maxs.y = std::copysignf(1.5f, player_maxs.y) + (.3f * 2);

		player_mins += player_origin;
		player_maxs += player_origin;

		ray_trace_t ray_trace(trace_origin, self_forward);

		float distance = 0.f;

		if (!ray_trace.trace(player_mins, player_maxs, distance))
			continue;

		if (distance < 6.0f)
			return true;
	}
	return false;
}

bool c_velocity::is_player_behind(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	const vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());
	const vec3 self_prev_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
	vec3 trace_origin = self_prev_origin + ((self_origin - self_prev_origin) * mc->get_render_partial_ticks());
	trace_origin.y += self->get_eye_height();

	const vec3 self_angle(self->get_pitch(), self->get_yaw() - 180.f, 0.f);
	vec3 self_forward;
	math::angle_vectors(self_angle, &self_forward);

	for (const auto& player : world->get_players())
	{
		if (!player || !player->player_instance || player->is_dead() || self->is_same(player) || !self->is_visible(player->player_instance))
			continue;

		const vec3 player_origin(player->origin_x(), player->origin_y(), player->origin_z());

		if ((self_origin - player_origin).length() > 8.0f)
			continue;

		vec3 player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
		vec3 player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());

		player_mins -= player_origin;
		player_maxs -= player_origin;

		player_mins *= 2.3;
		player_maxs *= 2.3;
		player_maxs.y = std::copysignf(1.5f, player_maxs.y) + (.3f * 2.3);

		player_mins += player_origin;
		player_maxs += player_origin;

		ray_trace_t ray_trace(trace_origin, self_forward);

		float distance = 0.f;

		if (!ray_trace.trace(player_mins, player_maxs, distance))
			continue;

		if (distance < 6.0f)
			return true;
	}
	return false;
}
