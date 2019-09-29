#include "context.h"
#include "aim_assist.h"

void c_aim_assist::find_best_point(const std::shared_ptr<c_player>& self, const std::shared_ptr<c_player>& player, const vec3& trace_origin, target_t& target_data)
{
	vec3 closest_point;
	std::vector<vec3> points;

	const vec3 player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
	const vec3 player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());

	const vec3 self_origin(self->origin_x(), 0.0, self->origin_z());
	const vec3 target_origin(player->origin_x(), 0.0, player->origin_z());

	const vec3 diff = self_origin - target_origin;

	if (ctx.m_settings.combat_aim_assist_multipoint && diff.length() > 2.5)
	{
		const vec3 player_origin(player->origin_x(), player->aabb_min_y(), player->origin_z());
		const vec3 head(player->origin_x(), player->aabb_min_y() + 1.62f, player->origin_z());

		vec3 player_aabb_diff = player_mins;

		player_aabb_diff -= player_origin;

		if (ctx.m_settings.combat_reach_hitboxes)
			player_aabb_diff *= ctx.m_settings.combat_reach_hitboxes_scale;

		auto divided_diff_x = player_aabb_diff.x / 2,
			divided_diff_z = player_aabb_diff.z / 2;

		points.emplace_back(head.x + player_aabb_diff.x, head.y, head.z);
		points.emplace_back(head.x - player_aabb_diff.x, head.y, head.z);
		points.emplace_back(head.x, head.y, head.z + player_aabb_diff.z);
		points.emplace_back(head.x, head.y, head.z - player_aabb_diff.z);
		points.emplace_back(head.x + player_aabb_diff.x, head.y, head.z + player_aabb_diff.z);
		points.emplace_back(head.x - player_aabb_diff.x, head.y, head.z - player_aabb_diff.z);
		points.emplace_back(head.x + player_aabb_diff.x, head.y, head.z - player_aabb_diff.z);
		points.emplace_back(head.x - player_aabb_diff.x, head.y, head.z + player_aabb_diff.z);

		//Extra points
		points.emplace_back(head.x + divided_diff_x, head.y, head.z);
		points.emplace_back(head.x - divided_diff_x, head.y, head.z);
		points.emplace_back(head.x, head.y, head.z + divided_diff_z);
		points.emplace_back(head.x, head.y, head.z - divided_diff_z);
		points.emplace_back(head.x + divided_diff_x, head.y, head.z + divided_diff_z);
		points.emplace_back(head.x - divided_diff_x, head.y, head.z - divided_diff_z);
		points.emplace_back(head.x + divided_diff_x, head.y, head.z - divided_diff_z);
		points.emplace_back(head.x - divided_diff_x, head.y, head.z + divided_diff_z);

	}
	else
		points.emplace_back(player->origin_x(), player->aabb_min_y() + 1.62f, player->origin_z());

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
			if (!player || !player->player_instance || player->is_dead() || self->is_same(player) || (!self->is_visible(player->player_instance) && ctx.m_settings.combat_aim_assist_visible_only))
				continue;

			if (ctx.m_settings.combat_aim_assist_invisibles && player->is_invisible())
				continue;

			if (ctx.m_settings.combat_aim_assist_nakeds && !player->has_armor())
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

void c_aim_assist::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	bool mouse_down = LI_FN(GetAsyncKeyState).cached()(VK_LBUTTON) < 0;

	if (!mc->in_game_has_focus() || ctx.m_menu_open)
		return;

	if (mc->is_hovering_block() && ctx.m_settings.combat_aim_assist_break_blocks)
		return;

	if (mc->is_in_inventory() || mc->is_in_chat())
		return;

	if (ctx.m_settings.combat_aim_assist_require_click && !mouse_down)
		return;

	bool white_list = false;

	if (ctx.m_settings.combat_aim_assist_item_whitelist)
	{
		if (ctx.m_settings.global_setting_axes && self->holding_axe())
			white_list = true;

		if (ctx.m_settings.global_setting_swords && self->holding_sword())
			white_list = true;

		if (ctx.m_settings.global_setting_hoes && self->holding_hoe())
			white_list = true;

		if (ctx.m_settings.global_setting_shovels && self->holding_shovel())
			white_list = true;

		if (ctx.m_settings.global_setting_pick_axes && self->holding_pick_axe())
			white_list = true;

		if (ctx.m_settings.global_setting_fishing_rod && self->holding_fishing_rod())
			white_list = true;

		if (!white_list)
			return;
	}

	jfloat yaw = self->get_yaw(),
		pitch = self->get_pitch();

	target_t target_data{};
	find_target(mc, self, world, target_data);

	if (!target_data.m_valid)
		return;

	auto random_h_scale = std::clamp(0.1f * ctx.m_settings.combat_aim_assist_h_speed, 0.01f, 0.5f);
	auto random_v_scale = std::clamp(0.1f * ctx.m_settings.combat_aim_assist_v_speed, 0.01f, 0.5f);

	auto random_h = 0.f;
	auto random_v = 0.f;

	if (util::random(0, 100) < 45)
	{
		random_h = util::random(-random_h_scale, random_h_scale);
		random_v = util::random(-random_v_scale, random_v_scale);
	}

	float sens = mc->get_mouse_sensitivity() * 0.6f + 0.2f;
	float step = pow(sens, 3) * 8.0f;

	float yaw_change = util::convert_legit_value(step, ((target_data.m_yaw_change / util::random(375.f, 425.f) * ctx.m_settings.combat_aim_assist_h_speed * 5.f) + random_h), ctx.m_settings.combat_aim_assist_h_speed);
	float pitch_change = util::convert_legit_value(step, ((target_data.m_pitch_change / util::random(375.f, 425.f) * ctx.m_settings.combat_aim_assist_v_speed * 5.f) + random_v), ctx.m_settings.combat_aim_assist_v_speed);

	if (abs(target_data.m_yaw_change) > 5 || (ctx.m_settings.combat_aim_assist_multipoint) && yaw_change > 0.00001)
		self->set_yaw(yaw + yaw_change);


	if (ctx.m_settings.combat_aim_assist_vertical && (abs(target_data.m_pitch_change) > 5 || ctx.m_settings.combat_aim_assist_multipoint) && pitch_change > 0.00001)
		self->set_pitch(pitch + pitch_change);

	
}
