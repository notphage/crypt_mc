#include "context.h"
#include "reach.h"

void c_reach::on_atan2(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (ctx.m_settings.combat_reach_on_sprint && !self->is_sprinting())
		return;

	if (ctx.m_settings.combat_reach_disable_in_water && self->in_water())
		return;

	if (util::random(1, 100) > ctx.m_settings.combat_reach_hitchance)
		return;

	std::shared_ptr<c_player> target_entity = nullptr;
	
	//Reach value
	float target_distance = ctx.m_settings.combat_reach_distance;

	const vec3 trace_origin(self->origin_x(), ctx.m_version == MC_VERSION::MC_18X ? self->origin_y() + self->get_eye_height() : self->origin_y(), self->origin_z());

	const vec3 self_angle(self->get_pitch(), self->get_yaw(), 0.f);
	vec3 self_forward;
	math::angle_vectors(self_angle, &self_forward);

	vec3 target_position{};
	
	for (const auto& player : world->get_players())
	{
	    if (!player || !player->player_instance || player->is_dead() || self->is_same(player) || (!self->is_visible(player->player_instance) && ctx.m_settings.combat_reach_visible))
	        continue;

		const vec3 self_origin(self->origin_x(), self->aabb_min_y(), self->origin_z());
		const vec3 player_origin(player->origin_x(), player->aabb_min_y(), player->origin_z());

		if (fabs(self_origin.y - player_origin.y) > 1.f && ctx.m_settings.combat_reach_ycheck)
			continue;

		vec3 player_mins(player->aabb_min_x(), player->aabb_min_y(), player->aabb_min_z());
		vec3 player_maxs(player->aabb_max_x(), player->aabb_max_y(), player->aabb_max_z());

		player_mins -= player_origin;
		player_maxs -= player_origin;

		if (ctx.m_settings.combat_reach_hitboxes)
		{
			player_mins *= ctx.m_settings.combat_reach_hitboxes_scale;
			player_maxs *= ctx.m_settings.combat_reach_hitboxes_scale;
			player_maxs.y = std::copysignf(1.5f, player_maxs.y) + (.3f * ctx.m_settings.combat_reach_hitboxes_scale);
		}
		
		player_mins += player_origin;
		player_maxs += player_origin;

		ray_trace_t ray_trace(trace_origin, self_forward);

		float distance = 0.f;
		vec3 temp_position{};
		if (!ray_trace.trace(player_mins, player_maxs, distance, temp_position))
			continue;

		if (distance < target_distance)
		{
			target_entity = player;
			target_distance = distance;
			target_position = temp_position;
		}
	}

	if (target_entity == nullptr || target_entity->player_instance == nullptr)
		return;

	mc->set_object_mouse_over(target_entity->player_instance, target_position);
	mc->set_pointed_entity(target_entity->player_instance);
}
