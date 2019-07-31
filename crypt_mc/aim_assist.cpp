#include "context.h"
#include "aim_assist.h"

//MC sens code
float c_aim_assist::clamp(float rotation) {
	float random_number = util::random(0.025, 0.04);

	if (rotation < random_number) 
		return random_number;
	
	return rotation;
}

float c_aim_assist::get_x_speed() const
{
	return ctx.m_settings.combat_aim_assist_h_speed + util::random(ctx.m_settings.combat_aim_assist_h_speed > .5f ? -.5f : -ctx.m_settings.combat_aim_assist_h_speed + .1f, .5f);
}

float c_aim_assist::get_y_speed() const
{
	return ctx.m_settings.combat_aim_assist_v_speed + util::random(ctx.m_settings.combat_aim_assist_v_speed > .5f ? -.5f : -ctx.m_settings.combat_aim_assist_v_speed + .1f, .5f);
}

void c_aim_assist::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.combat_aim_assist || !valid_keystate(ctx.m_settings.combat_aim_assist_key))
		return;

	bool mouse_down = LI_FN(GetAsyncKeyState).cached()(VK_LBUTTON) < 0;

	if (!mc->in_game_has_focus() || ctx.m_menu_open)
		return;

	if (mc->is_in_inventory() || mc->is_in_chat())
		return;

	if (ctx.m_settings.combat_aim_assist_require_click && !mouse_down)
		return;
	
	if (ctx.m_settings.combat_aim_assist_weapons_only && !self->holding_weapon())
		return;

	static std::shared_ptr<c_player> target_entity = nullptr;

	jdouble target_distance = ctx.m_settings.combat_aim_assist_distance;

	auto target_yaw_change = 90.f, target_pitch_change = 90.f;
	jfloat yaw = 0.f, pitch = 0.f;

	const vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());

	auto determine_target = [&](const std::shared_ptr<c_player>& player) -> bool
	{
		if (!player || !player->player_instance || player->is_dead() || self->is_same(player))
			return false;

		if (!ctx.m_settings.combat_aim_assist_invisibles && player->is_invisible())
			return false;

		if (!ctx.m_settings.combat_aim_assist_nakeds && !player->has_armor())
			return false;

		pitch = self->get_pitch();
		yaw = self->get_yaw();

		const vec3 head(player->origin_x(), player->origin_y() + 1.2f, player->origin_z());
		vec3 diff = head - self_origin;

		auto hypoteneuse = diff.length();

		if (hypoteneuse > target_distance)
			return false;

		auto p_pitch = -atan2(diff.y, hypoteneuse) * 180.f / math::pi;
		auto p_yaw = atan2(diff.z, diff.x) * 180.0f / math::pi - 90.0F;
		auto pitch_change = math::wrap_angle_to_180(p_pitch - pitch);
		auto yaw_change = math::wrap_angle_to_180(p_yaw - yaw);

		if (abs(yaw_change) < ctx.m_settings.combat_aim_assist_fov && (abs(pitch_change) < ctx.m_settings.combat_aim_assist_fov || !ctx.m_settings.combat_aim_assist_vertical))
		{
			target_entity = player;
			target_distance = hypoteneuse;
			target_pitch_change = pitch_change;
			target_yaw_change = yaw_change;

			return true;
		}

		return false;
	};

	bool found_target = false;

	if (ctx.m_settings.combat_aim_assist_sticky && target_entity)
	{
		found_target = determine_target(target_entity);
	}
	
	if (!found_target)
	{
		for (auto player : world->get_players())
		{
			found_target = determine_target(player);

			if (found_target)
				break;
		}

		if (!found_target)
		{
			target_entity = nullptr;

			return;
		}
	}

	if (abs(target_yaw_change) > 5)
		self->set_yaw(yaw + (target_yaw_change / util::random(445, 455)) * get_x_speed());

	if (ctx.m_settings.combat_aim_assist_vertical)
		if (abs(target_pitch_change) > 5)
			self->set_pitch(pitch + (target_pitch_change / util::random(445, 455)) * get_y_speed());

	if ((rand() % 100) <= 2)
	{
		self->set_pitch(pitch + util::random(0, 1) == 1 ? -clamp(util::random(0.02f, 0.1f)) : clamp(util::random(0.02f, 0.1f)));
		self->set_yaw(yaw + util::random(0, 1) == 1 ? -clamp(util::random(0.02f, 0.1f)) : clamp(util::random(0.02f, 0.1f)));
	}
}
