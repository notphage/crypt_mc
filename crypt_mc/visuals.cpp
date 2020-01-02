#include "context.h"
#include "visuals.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"

bool c_visuals::esp_box_t::create_player(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& ent)
{
	jfloat render_partial_ticks = mc->get_render_partial_ticks();
	jfloat ent_height = ent->get_height();

	vec3 min(ent->aabb_min_x(), ent->aabb_min_y(), ent->aabb_min_z());
	vec3 max(ent->aabb_max_x(), ent->aabb_max_y(), ent->aabb_max_z());

	vec3 origin(ent->origin_x(), ent->origin_y(), ent->origin_z());
	min -= origin;
	max -= origin;
	max.y =  (ent_height / 2.f);
	min.y = -(ent_height / 2.f);

	vec3 prev_origin(ent->old_origin_x(), ent->old_origin_y(), ent->old_origin_z());
	m_interp_origin = prev_origin + ((origin - prev_origin) * render_partial_ticks);

	m_center = m_interp_origin + ((min + max) * 0.5f);
	m_center.y += (ent_height / 1.75f);

	m_bbox =
	{
		vec3(min.x, min.y, min.z),
		vec3(min.x, max.y, min.z),
		vec3(max.x, max.y, min.z),
		vec3(max.x, min.y, min.z),
		vec3(min.x, min.y, max.z),
		vec3(min.x, max.y, max.z),
		vec3(max.x, max.y, max.z),
		vec3(max.x, min.y, max.z)
	};

	vec2 temp_point;
	for (auto&& point : m_bbox)
	{
		if (!ctx.w2s(m_center + point, temp_point))
			return false;

		minx = math::min(minx, static_cast<int32_t>(temp_point.x));
		maxx = math::max(maxx, static_cast<int32_t>(temp_point.x));
		miny = math::min(miny, static_cast<int32_t>(temp_point.y));
		maxy = math::max(maxy, static_cast<int32_t>(temp_point.y));
	}

	if (!ctx.w2s(m_center, temp_point))
		return false;

	centerx = temp_point.x;
	centery = temp_point.y;

	return true;
}

void c_visuals::draw_ent_box(const color_t& color, esp_box_t& box, bool outlined, const color_t& outline_color)
{
	float x = box.minx;
	float y = box.miny;
	float w = box.maxx - x;
	float h = box.maxy - y;

	if (outlined)
	{
		ctx.m_renderer->draw_rect({ x + 1.f, y + 1.f, w - 2.f, h - 2.f }, outline_color);
		ctx.m_renderer->draw_rect({ x - 1.f, y - 1.f, w + 2.f, h + 2.f }, outline_color);
	}

	ctx.m_renderer->draw_rect({ x, y, w, h }, color);
}

void c_visuals::draw_ent_3d_box(const color_t& color, esp_box_t& box, float yaw, bool outlined, const color_t& outline_color)
{
	for (vec3& point : box.m_bbox)
		math::rotate2d(point, yaw);

	std::array<vec2, 8> corners;

	{
		vec2 temp_point;
		for (int i = 0; i <= 7; i++)
		{
			if (!ctx.w2s(box.m_center + box.m_bbox[i], temp_point))
				return;

			corners[i] = temp_point;
		}
	}

	for (auto i = 1; i <= 4; i++)
	{
		ctx.m_renderer->draw_line({ corners[i - 1].x, corners[i - 1].y }, { corners[i % 4].x,	  corners[i % 4].y }, color);
		ctx.m_renderer->draw_line({ corners[i - 1].x, corners[i - 1].y }, { corners[i + 3].x,	  corners[i + 3].y }, color);
		ctx.m_renderer->draw_line({ corners[i + 3].x, corners[i + 3].y }, { corners[i % 4 + 4].x, corners[i % 4 + 4].y }, color);

		if (outlined)
		{
			if (i % 2 == 0)
			{
				ctx.m_renderer->draw_line({ corners[i - 1].x, corners[i - 1].y + 1.f }, { corners[i % 4].x, corners[i % 4].y + 1.f }, outline_color);
				ctx.m_renderer->draw_line({ corners[i - 1].x, corners[i - 1].y - 1.f }, { corners[i % 4].x, corners[i % 4].y - 1.f }, outline_color);

				ctx.m_renderer->draw_line({ corners[i + 3].x, corners[i + 3].y + 1.f }, { corners[i % 4 + 4].x, corners[i % 4 + 4].y + 1.f }, outline_color);
				ctx.m_renderer->draw_line({ corners[i + 3].x, corners[i + 3].y - 1.f }, { corners[i % 4 + 4].x, corners[i % 4 + 4].y - 1.f }, outline_color);
			}
			else
			{
				ctx.m_renderer->draw_line({ corners[i - 1].x + 1.f, corners[i - 1].y }, { corners[i % 4].x + 1.f , corners[i % 4].y }, outline_color);
				ctx.m_renderer->draw_line({ corners[i - 1].x - 1.f, corners[i - 1].y }, { corners[i % 4].x - 1.f , corners[i % 4].y }, outline_color);

				ctx.m_renderer->draw_line({ corners[i + 3].x + 1.f, corners[i + 3].y }, { corners[i % 4 + 4].x + 1.f, corners[i % 4 + 4].y }, outline_color);
				ctx.m_renderer->draw_line({ corners[i + 3].x - 1.f, corners[i + 3].y }, { corners[i % 4 + 4].x - 1.f, corners[i % 4 + 4].y }, outline_color);
			}

			ctx.m_renderer->draw_line({ corners[i - 1].x + 1.f, corners[i - 1].y }, { corners[i + 3].x + 1.f, corners[i + 3].y }, outline_color);
			ctx.m_renderer->draw_line({ corners[i - 1].x - 1.f, corners[i - 1].y }, { corners[i + 3].x - 1.f, corners[i + 3].y }, outline_color);
		}
	}
}

void c_visuals::draw_ent_corner_box(const color_t& color, esp_box_t& box, bool outlined, const color_t& outline_color)
{
	float x = box.minx;
	float y = box.miny;
	float w = box.maxx - x;
	float h = box.maxy - y;

	float linew = (w / 4.f);
	float lineh = (w / 5.f);

	if (outlined)
	{
		// top left
		ctx.m_renderer->draw_line({ x - 1.f, y - 1.f }, { x + linew, y - 1.f }, outline_color);
		ctx.m_renderer->draw_line({ x - 1.f, y - 1.f }, { x - 1.f, y + lineh }, outline_color);

		// bot left
		ctx.m_renderer->draw_line({ x - 1.f, y + h - lineh }, { x - 1.f, y + h + 2.f }, outline_color);
		ctx.m_renderer->draw_line({ x - 1.f, y + h + 1.f }, { x + linew, y + h + 1.f }, outline_color);

		// top right
		ctx.m_renderer->draw_line({ x + w - linew, y - 1.f }, { x + w + 1.f, y - 1.f }, outline_color);
		ctx.m_renderer->draw_line({ x + w + 1.f, y - 1.f }, { x + w + 1.f, y + lineh }, outline_color);

		// bot right
		ctx.m_renderer->draw_line({ x + w + 1.f, y + h - lineh }, { x + w + 1.f, y + h + 1.f }, outline_color);
		ctx.m_renderer->draw_line({ x + w - linew, y + h + 1.f }, { x + w + 1.f, y + h + 1.f }, outline_color);
	}

	// top left
	ctx.m_renderer->draw_line({ x, y }, { x, y + lineh }, color);
	ctx.m_renderer->draw_line({ x, y }, { x + linew, y }, color);

	// top right
	ctx.m_renderer->draw_line({ x + w - linew, y }, { x + w, y }, color);
	ctx.m_renderer->draw_line({ x + w, y }, { x + w, y + lineh }, color);

	// bot left
	ctx.m_renderer->draw_line({ x, y + h - lineh }, { x, y + h + 1.f }, color);
	ctx.m_renderer->draw_line({ x, y + h }, { x + linew, y + h }, color);

	// bot right
	ctx.m_renderer->draw_line({ x + w - linew, y + h }, { x + w, y + h }, color);
	ctx.m_renderer->draw_line({ x + w, y + h - lineh }, { x + w, y + h }, color);
}

void c_visuals::draw_ent_filled_box(const color_t& color, esp_box_t& box)
{
	float x = box.minx;
	float y = box.miny;
	float w = box.maxx - x;
	float h = box.maxy - y;

	ctx.m_renderer->draw_filled_rect({ x - 2.f, y - 2.f, w + 4.f, h + 4.f }, color);
}

void c_visuals::on_swap_buffers(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	std::vector<esp_player_t> players;

	for (auto&& player : world->get_players())
	{
		if (player->is_same(self) || player->get_health() <= 0 || player->is_dead())
			continue;
	
		vec3 self_origin(self->origin_x(), self->origin_y(), self->origin_z());
		vec3 player_origin(player->origin_x(), player->origin_y(), player->origin_z());
	
		float distance = self_origin.dist(player_origin);
	
		players.emplace_back(player, distance);
	}

	std::sort(players.begin(), players.end(), [](const esp_player_t& a, const esp_player_t& b) -> bool
		{
			return a.m_dist > b.m_dist;
		});
	
	for (auto&& esp_player : players)
	{
		esp_player.draw(m_jni, mc, self, world);
	}
}

void c_visuals::esp_player_t::draw(JNIEnv* jni, const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	esp_box_t esp_box;
	color_t box_color, box_filled_color, name_color, box_outline_color, snaplines_outline_color;
	uint8_t alpha, alpha_flags;
	bool box_outlined, snaplines_outlined, is_invisible, is_naked, is_sneaking, is_bot;

	if (!esp_box.create_player(mc, m_player))
		return;

	box_outlined = ctx.m_settings.visuals_esp_box_outlined;
	snaplines_outlined = ctx.m_settings.visuals_esp_snap_lines_outlined;

	// setup colors
	box_color = ctx.m_settings.visuals_esp_box_color;
	box_filled_color = ctx.m_settings.visuals_esp_box_filled_color;
	box_outline_color = ctx.m_settings.visuals_esp_box_outlined_color;
	snaplines_outline_color = ctx.m_settings.visuals_esp_snap_lines_outlined_color;
	name_color = color_t(255, 255, 255, 255);

	// setup flags
	is_invisible = m_player->is_invisible();
	is_naked = m_player->has_armor();
	is_sneaking = m_player->is_invisible();
	is_bot = false; // m_player->is_bot();

	if (is_invisible)
		name_color = color_t(85, 255, 255, 255);
	else if (is_sneaking)
		name_color = color_t(255, 85, 85, 255);

	alpha = 175;
	alpha_flags = 125;

	// TODO: dormant system

	box_outline_color.a(alpha);
	snaplines_outline_color.a(alpha);

	if (ctx.m_settings.visuals_esp_box)
	{
		if ((is_invisible && !ctx.m_settings.visuals_esp_box_invisible) || (is_naked && !ctx.m_settings.visuals_esp_box_nakeds) || (is_bot && !ctx.m_settings.visuals_esp_box_bots))
			goto esp_box_skip;

		switch (ctx.m_settings.visuals_esp_box_mode)
		{
			case 0:
			{
				if (ctx.m_settings.visuals_esp_box_filled)
					draw_ent_filled_box(box_filled_color, esp_box);

				draw_ent_box(box_color, esp_box, box_outlined, box_outline_color);

				break;
			}
			
			case 1:
			{
				if (ctx.m_settings.visuals_esp_box_filled)
					draw_ent_filled_box(box_filled_color, esp_box);

				draw_ent_corner_box(box_color, esp_box, box_outlined, box_outline_color);

				break;
			}

			case 2:
			{
				draw_ent_3d_box(box_color, esp_box, m_player->get_yaw(), box_outlined, box_outline_color);
				break;
			}
		}
	}

esp_box_skip:

	if (ctx.m_settings.visuals_esp_names)
	{
		jstring j_name = m_player->get_name();
		const char* char_string = jni->GetStringUTFChars(j_name, nullptr);
		std::string contain_string(char_string);

		jni->ReleaseStringUTFChars(j_name, char_string);
		jni->DeleteLocalRef(j_name);

		ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { (esp_box.maxx + esp_box.minx) / 2.f, esp_box.miny - 14.0f }, contain_string, name_color, TEXT_CENTER);
	}

	if (ctx.m_settings.visuals_esp_healthbar)
		draw_health({ esp_box.minx, esp_box.maxy }, { esp_box.minx, esp_box.miny }, (int)m_player->get_health(), (int)m_player->get_max_health(), alpha);

	if (ctx.m_settings.visuals_esp_snap_lines && (ctx.m_settings.visuals_esp_snap_lines_distance ? m_dist < ctx.m_settings.visuals_esp_snap_lines_distance_blocks : true))
	{
		if ((is_invisible && !ctx.m_settings.visuals_esp_snap_lines_invisible) || (is_naked && !ctx.m_settings.visuals_esp_snap_lines_nakeds) || (is_bot && !ctx.m_settings.visuals_esp_snap_lines_bots))
			return; // goto esp_snap_line_skip;

		draw_snaplines(mc, self, { (esp_box.maxx + esp_box.minx) / 2.f, (float)esp_box.maxy }, snaplines_outlined, snaplines_outline_color);
	}

//esp_snap_line_skip:
}

void c_visuals::esp_player_t::draw_health(const vec2& bot, const vec2& top, int health, int max_health, uint8_t alpha)
{
	color_t outline_color(0, 0, 0, 175);

	int h = math::clamp< int >(health, 0, max_health);

	float fraction = h * 0.05f;
	color_t color(math::min(255 * (max_health - health) / max_health, 255), 255.f * fraction, 0, alpha);

	float height = bot.y - top.y;
	float pos = height - (height * fraction);

	ctx.m_renderer->draw_line({ top.x - 6.f, top.y - 1.f }, { top.x - 6.f, bot.y + 2.f }, outline_color);
	ctx.m_renderer->draw_line({ top.x - 5.f, top.y - 1.f }, { top.x - 5.f, bot.y + 2.f }, outline_color);
	ctx.m_renderer->draw_line({ top.x - 4.f, top.y - 1.f }, { top.x - 4.f, bot.y + 2.f }, outline_color);
	ctx.m_renderer->draw_line({ top.x - 3.f, top.y - 1.f }, { top.x - 3.f, bot.y + 2.f }, outline_color);

	ctx.m_renderer->draw_line({ top.x - 5.f, top.y + pos }, { top.x - 5.f, bot.y + 1.f }, color);
	ctx.m_renderer->draw_line({ top.x - 4.f, top.y + pos }, { top.x - 4.f, bot.y + 1.f }, color);

	if (health != max_health)
		ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { top.x - 5.f, top.y + pos - 5.0f }, std::to_string(health), color_t(255, 255, 255, 255), TEXT_CENTER);
}

void c_visuals::esp_player_t::draw_snaplines(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const vec2& end, bool outlined, const color_t& outline_color)
{
	vec2 begin{};

	switch (ctx.m_settings.visuals_esp_snap_lines_origin)
	{
		case 0:
		{
			begin.x = (float)ctx.m_screen_w / 2.f;
			begin.y = (float)ctx.m_screen_h;

			break;
		}

		case 1:
		{
			begin.x = (float)ctx.m_screen_w / 2.f;
			begin.y = (float)ctx.m_screen_h / 2.f;

			break;
		}

		case 2:
		{
			vec3 self_origin(self->origin_x(), self->aabb_min_y(), self->origin_z());
			vec3 self_prev_origin(self->old_origin_x(), self->aabb_min_y(), self->old_origin_z());
			vec3 self_interp_origin = self_prev_origin + ((self_origin - self_prev_origin) * mc->get_render_partial_ticks());

			ctx.w2s(self_interp_origin, begin);

			break;
		}
	}

	color_t snaplines_color{};
	switch (ctx.m_settings.visuals_esp_snap_lines_mode)
	{
		case 0: // static
		{
			snaplines_color = ctx.m_settings.visuals_esp_snap_lines_color;

			break;
		}

		case 1: // distance
		{
			float dist = m_dist;
			constexpr float max_dist = 64.f;
			float h = math::clamp<float>(dist, 0.f, max_dist) / max_dist;

			float near_color = ctx.m_settings.visuals_esp_snap_lines_near_color().to_hue();
			float far_color = ctx.m_settings.visuals_esp_snap_lines_far_color().to_hue();

			snaplines_color.from_hsv(math::lerp(far_color, near_color, h), 1.f, 1.f);
			snaplines_color.a(255);

			break;
		}

		case 2: // health
		{
			int health = m_player->get_health();
			int max_health = m_player->get_max_health();
			int h = math::clamp<int>(health, 0, max_health);

			float fraction = h * 0.05f;
			snaplines_color = { math::min(255 * (max_health - health) / max_health, 255), (int)(255.f * fraction), 0, 255 };

			break;
		}
	}

	if (outlined)
	{
		ctx.m_renderer->draw_line({ begin.x + 1.f, begin.y }, { end.x - 1.f, end.y + 1.f }, outline_color);
		ctx.m_renderer->draw_line({ begin.x - 1.f, begin.y }, { end.x + 1.f, end.y + 1.f }, outline_color);
	}
															   
	ctx.m_renderer->draw_line(begin, end, snaplines_color);
}
