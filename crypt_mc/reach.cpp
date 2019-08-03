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
	
	std::shared_ptr<c_player> target_entity = nullptr;
	
	//Reach value
	auto target_distance = util::random(ctx.m_settings.combat_reach_distance_min(), ctx.m_settings.combat_reach_distance_max());

	const vec3 player_origin(self->origin_x(), self->origin_y(), self->origin_z());
	
	for (auto player : world->get_players())
	{
	    if (!player || !player->player_instance || player->is_dead()|| self->is_same(player))
	        continue;
	
		const vec3 origin(player->origin_x(), player->origin_y(), player->origin_z());
		const vec3 diff = origin - player_origin;

		auto distance = sqrt(diff.x * diff.x + diff.z * diff.z);

		if (distance > target_distance)
			continue;
	
        float eye_pos = (player->origin_y() + player->get_eye_height()) - (self->origin_y() + self->get_eye_height());

        float xz_distance = sqrt(
            diff.x * diff.x +
            diff.z * diff.z);

        float perfect_pitch = -(atan2f(eye_pos - 0.75f, xz_distance) * 180.0f / math::pi);
        float pitch_change = math::normalize(perfect_pitch - self->get_pitch());

        float scaled_v = distance > 3.0f ? 16.6f - ((distance - 3.0f) / 0.38f) : 16.6f;

        if (abs(pitch_change) > scaled_v)
            continue;

	    auto p_yaw = atan2(diff.z, diff.x) * 180.0f / math::pi - 90.0F;
	    auto yaw_change = math::normalize(p_yaw - self->get_yaw());

        float scaled = (distance > 3.0f ? 7.0f - ((distance - 3.0f) / 1.2f) : 7.0f) * (ctx.m_settings.combat_reach_hitboxes ? ctx.m_settings.combat_reach_hitboxes_scale : 1.f);

	    if (abs(yaw_change) < scaled)
	    {
			if (ctx.m_settings.combat_reach_y_check && fabs(player->origin_y() - self->origin_y()) > .33f)
				continue;

			target_entity = player;
			target_distance = distance;
	    }
	}

	if (target_entity == nullptr || target_entity->player_instance == nullptr)
		return;

	mc->set_object_mouse_over(target_entity->player_instance);
	mc->set_pointed_entity(target_entity->player_instance);
}
