#include "context.h"
#include "aim_assist.h"

void c_aim_assist::find_best_point(const std::shared_ptr<c_player>& self, const std::shared_ptr<c_player>& player, const vec3& trace_origin, target_t& target_data)
{
	vec3 closest_point;
	std::vector<vec3> points;

	const vec3 player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
	const vec3 player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());
	
	if (ctx.m_settings.combat_aim_assist_multipoint)
	{
		const vec3 head(player->origin_x(), player->origin_y() + 1.2f, player->origin_z());
		const vec3 player_origin(player->origin_x(), player->origin_y(), player->origin_z());

		vec3 player_aabb_diff = player_mins;

		player_aabb_diff -= player_origin;

		if (ctx.m_settings.combat_reach_hitboxes)
			player_aabb_diff *= ctx.m_settings.combat_reach_hitboxes_scale;

		points.emplace_back(head.x + player_aabb_diff.x, head.y, head.z);
		points.emplace_back(head.x - player_aabb_diff.x, head.y, head.z);
		points.emplace_back(head.x, head.y, head.z + player_aabb_diff.z);
		points.emplace_back(head.x, head.y, head.z - player_aabb_diff.z);
		points.emplace_back(head.x + player_aabb_diff.x, head.y, head.z + player_aabb_diff.z);
		points.emplace_back(head.x - player_aabb_diff.x, head.y, head.z - player_aabb_diff.z);
		points.emplace_back(head.x + player_aabb_diff.x, head.y, head.z - player_aabb_diff.z);
		points.emplace_back(head.x - player_aabb_diff.x, head.y, head.z + player_aabb_diff.z);
	}
	else
		points.emplace_back(player->origin_x(), player->origin_y() + 1.2f, player->origin_z());

	float pitch = self->get_pitch(), yaw = self->get_yaw();
	float best_distance = ctx.m_settings.combat_aim_assist_distance;
	for (const auto& point : points)
	{
		vec3 diff = point - trace_origin;
		auto hypoteneuse = sqrt(diff.x * diff.x + diff.z * diff.z);

		auto p_pitch = -atan2(diff.y, hypoteneuse) * 180.f / math::pi;
		auto p_yaw = atan2(diff.z, diff.x) * 180.0f / math::pi - 90.0F;
		vec3 angle_to_point(p_pitch, p_yaw, 0.f);
		vec3 dir_to_point;
		math::angle_vectors(angle_to_point, &dir_to_point);

		ray_trace_t ray_trace(trace_origin, dir_to_point);

		float distance = 0.f;
		if (!ray_trace.trace(player_mins, player_maxs, distance))
			continue;

		auto pitch_change = math::wrap_angle_to_180(p_pitch - pitch);
		auto yaw_change = math::wrap_angle_to_180(p_yaw - yaw);

		if (distance <= best_distance && (abs(yaw_change) < ctx.m_settings.combat_aim_assist_fov && (abs(pitch_change) < ctx.m_settings.combat_aim_assist_fov || !ctx.m_settings.combat_aim_assist_vertical)))
		{
			best_distance = distance;

			target_data.m_valid = true;
			target_data.m_target = player;
			target_data.m_distance = distance;
			target_data.m_pitch_change = pitch_change;
			target_data.m_yaw_change = yaw_change;
		}
	}
}

void c_aim_assist::find_target(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world, target_t& best_target_data)
{
	static std::shared_ptr<c_player> target_entity = nullptr;

	const vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());
	const vec3 self_prev_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
	vec3 trace_origin = self_prev_origin + ((self_origin - self_prev_origin) * mc->get_render_partial_ticks());
	trace_origin.y += self->get_eye_height();

	if (ctx.m_settings.combat_aim_assist_sticky && target_entity != nullptr)
		find_best_point(self, target_entity, trace_origin, best_target_data);
	
	float best_distance = ctx.m_settings.combat_aim_assist_distance;
	if (!best_target_data.m_valid)
	{
		target_entity = nullptr;
		for (const auto& player : world->get_players())
		{
			if (!player || !player->player_instance || player->is_dead() || self->is_same(player) || !self->is_visible(player->player_instance))
				continue;

			if (!ctx.m_settings.combat_aim_assist_invisibles && player->is_invisible())
				continue;

			if (!ctx.m_settings.combat_aim_assist_nakeds && !player->has_armor())
				continue;

			target_t target_data{};
			find_best_point(self, player, trace_origin, target_data);

			if (target_data.m_valid && target_data.m_distance <= best_distance)
			{
				best_distance = target_data.m_distance;
				best_target_data = target_data;
			}
		}
	}
}

//MC sens code
void c_aim_assist::scan()
{
	m_vec.clear();

	for (auto d = 0; d < 30; d++)
	{
		jfloat sens = (ctx.m_settings.combat_aim_assist_scale * 0.5f) * 0.6f * 0.6f + 0.2f;

		jfloat rsens = sens * sens * sens * 8.0f;
		jfloat delta = d * rsens * 0.15f;

		m_vec.push_back(delta);
	}
}

float c_aim_assist::get_x_speed()
{
	auto value = ctx.m_settings.combat_aim_assist_h_speed + util::random(-1.f, 1.f);

	return m_vec.at(std::clamp(value, 1.f, 15.f));
}

float c_aim_assist::get_y_speed()
{
	auto value = ctx.m_settings.combat_aim_assist_v_speed + util::random(-1.f, 1.f);

	return m_vec.at(std::clamp(value, 1.f, 15.f));
}

void c_aim_assist::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_aim_assist || !valid_keystate(ctx.m_settings.combat_aim_assist_key))
		return;

	if (m_last_scale == -1.f || ctx.m_settings.combat_aim_assist_scale != m_last_scale)
	{
		scan();

		m_last_scale = ctx.m_settings.combat_aim_assist_scale;
	}

	m_last_scale = ctx.m_settings.combat_aim_assist_scale;

	bool mouse_down = LI_FN(GetAsyncKeyState).cached()(VK_LBUTTON) < 0;

	if (!mc->in_game_has_focus() || ctx.m_menu_open)
		return;

	if (mc->is_in_inventory() || mc->is_in_chat())
		return;

	if (ctx.m_settings.combat_aim_assist_require_click && !mouse_down)
		return;

	if (ctx.m_settings.combat_aim_assist_weapons_only && !self->holding_weapon())
		return;

	jfloat yaw = self->get_yaw(), pitch = self->get_pitch();

	target_t target_data{};
	find_target(mc, self, world, target_data);

	if (!target_data.m_valid)
		return;

	if (abs(target_data.m_yaw_change) > m_vec.at(1))
		if (abs(target_data.m_yaw_change) > 5 || ctx.m_settings.combat_aim_assist_multipoint)
			self->set_yaw(yaw + (target_data.m_yaw_change < 0 ? -get_x_speed() : get_x_speed()));

	if (ctx.m_settings.combat_aim_assist_vertical)
		if (abs(target_data.m_pitch_change) > m_vec.at(1))
			if (abs(target_data.m_pitch_change) > 5 || ctx.m_settings.combat_aim_assist_multipoint)
				self->set_pitch(pitch + (target_data.m_pitch_change < 0 ? -get_y_speed() : get_y_speed()));

	if (util::random(0, 50) <= 1)
	{
		self->set_pitch(pitch + (util::random(-m_vec.at(1), m_vec.at(1))));
		self->set_yaw(yaw + (util::random(-m_vec.at(1), m_vec.at(1))));
	}
}