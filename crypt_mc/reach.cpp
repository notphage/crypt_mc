#include "context.h"
#include "reach.h"

void c_reach::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_reach || !valid_keystate(ctx.m_settings.combat_reach_key))
	    return;
	
	if (ctx.m_settings.combat_reach_on_sprint && !self->is_sprinting())
		return;

	if (ctx.m_settings.combat_reach_disable_in_water && self->in_water())
		return;

	if (util::random(1, 100) > ctx.m_settings.combat_reach_hitchance)
		return;

	std::shared_ptr<c_player> target_entity = nullptr;
	
	//Reach value
	float target_distance = ctx.m_settings.combat_reach_distance;

	const vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());
	const vec3 self_prev_origin(self->old_origin_x(), self->old_origin_y(), self->old_origin_z());
	vec3 trace_origin = self_prev_origin + ((self_origin - self_prev_origin) * mc->get_render_partial_ticks());
	trace_origin.y += self->get_eye_height();

	const vec3 self_angle(self->get_pitch(), self->get_yaw(), 0.f);
	vec3 self_forward;
	math::angle_vectors(self_angle, &self_forward);
	
	for (auto player : world->get_players())
	{
	    if (!player || !player->player_instance || player->is_dead()|| self->is_same(player))
	        continue;

		const vec3 player_origin(player->origin_x(), player->origin_y(), player->origin_z());

		if (ctx.m_settings.combat_reach_y_check && fabs(self_origin.y - player_origin.y) > 0.2f)
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
		if (!ray_trace.trace(player_mins, player_maxs, distance))
			continue;

		if (distance < target_distance)
		{
			target_entity = player;
			target_distance = distance;
		}
	}

	if (target_entity == nullptr || target_entity->player_instance == nullptr)
		return;

	mc->set_object_mouse_over(target_entity->player_instance);
	mc->set_pointed_entity(target_entity->player_instance);
}
