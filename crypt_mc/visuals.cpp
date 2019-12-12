#include "context.h"
#include "visuals.h"
#include "glm/glm.hpp"
#include "glm/gtx/rotate_vector.hpp"

bool c_visuals::esp_box_t::create_player(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_player>& ent)
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
	vec3 interp_origin = prev_origin + ((origin - prev_origin) * render_partial_ticks);

	vec3 center = interp_origin + ((min + max) * 0.5f);
	center.y += (ent_height / 1.75f);

	vec3 points[] =
	{
		vec3(min.x, min.y, min.z),
		vec3(min.x, max.y, min.z),
		vec3(max.x, max.y, min.z),
		vec3(max.x, min.y, min.z),
		vec3(max.x, max.y, max.z),
		vec3(min.x, max.y, max.z),
		vec3(min.x, min.y, max.z),
		vec3(max.x, min.y, max.z)
	};

	vec2 temp_point;
	for (auto&& point : points)
	{
		if (!ctx.w2s(center + point, temp_point))
			return false;

		minx = math::min(minx, static_cast<int32_t>(temp_point.x));
		maxx = math::max(maxx, static_cast<int32_t>(temp_point.x));
		miny = math::min(miny, static_cast<int32_t>(temp_point.y));
		maxy = math::max(maxy, static_cast<int32_t>(temp_point.y));
	}

	if (!ctx.w2s(center, temp_point))
		return false;

	centerx = temp_point.x;
	centery = temp_point.y;

	return true;
}

void c_visuals::draw_ent_box(const color_t& color, esp_box_t& box, bool outlined = true, const color_t& outline_color = color_t(0, 0, 0, 175))
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

void c_visuals::draw_ent_corner_box(const color_t& color, esp_box_t& box, bool outlined = true, const color_t& outline_color = color_t(0, 0, 0, 175))
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
		ctx.m_renderer->draw_line({ x - 1.f, y - 1.f }, { x + linew, y - 1.f }, outline_color);

		// bot left
		ctx.m_renderer->draw_line({ x - 1.f, y + h - lineh }, { x - 1.f, y + h + 1.f }, outline_color);
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
	ctx.m_renderer->draw_line({ x, y + h - lineh }, { x, y + h }, color);
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
	
		if (player->is_invisible() && !ctx.m_settings.visuals_esp_invisible)
			continue;
	
		//if (player->is_bot() && !ctx.m_settings.visuals_esp_bots)
		//	continue;
	
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
	color_t box_color, box_filled_color, snapline_color, name_color, outline_color;
	uint8_t alpha, alpha_flags;

	if (!esp_box.create_player(mc, self, m_player))
		return;

	// setup colors
	box_color = ctx.m_settings.visuals_esp_box_color;
	box_filled_color = ctx.m_settings.visuals_esp_box_filled_color;
	snapline_color = ctx.m_settings.visuals_esp_snap_lines_color;
	name_color = color_t(255, 255, 255, 255);

	if (m_player->is_invisible())
		name_color = color_t(85, 255, 255, 255);
	else if (m_player->is_sneaking())
		name_color = color_t(255, 85, 85, 255);

	outline_color = color_t(0, 0, 0, 175);
	alpha = 175;
	alpha_flags = 125;

	// TODO: dormant system

	outline_color.a(alpha);

	if (ctx.m_settings.visuals_esp_box)
	{
		switch (ctx.m_settings.visuals_esp_box_mode)
		{
			case 0:
			{
				draw_ent_box(box_color, esp_box);
				break;
			}
			
			case 1:
			{
				draw_ent_corner_box(box_color, esp_box);
				break;
			}
		}

		if (ctx.m_settings.visuals_esp_box_filled)
		{
			draw_ent_filled_box(box_filled_color, esp_box);
		}
	}

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
		draw_health({ esp_box.minx, esp_box.maxy }, { esp_box.minx, esp_box.miny }, (int)m_player->get_health(), (int)m_player->get_max_health(), alpha, outline_color);

	//if (ctx.m_settings.visuals_esp_snap_lines)
	//	draw_snaplines({ (esp_box.maxx + esp_box.minx) / 2.f, (float)esp_box.maxy }, snapline_color, outline_color);
}

void c_visuals::esp_player_t::draw_health(const vec2& bot, const vec2& top, int health, int max_health, uint8_t alpha, const color_t& outline_color)
{
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
	{
		//ctx.m_surface.string(surface_font_t::sfont_esp_small, color_t(255, 255, 255, outline_color.a()), top.x - 5.f, top.y + pos - 5.0f, true, xors("%i"), health);
		ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { top.x - 5.f, top.y + pos - 5.0f }, std::to_string(health), color_t(255, 255, 255, 255), TEXT_CENTER);
	}
}

void c_visuals::esp_player_t::draw_snaplines(const vec2& bot, const color_t& snaplines_color, const color_t& outline_color)
{
	float bot_middle_screen = ctx.m_screen_w / 2.f;

	ctx.m_renderer->draw_line({ bot_middle_screen + 1.f, (float)ctx.m_screen_h }, { bot.x - 1.f, bot.y + 1.f }, outline_color);
	ctx.m_renderer->draw_line({ bot_middle_screen - 1.f, (float)ctx.m_screen_h }, { bot.x + 1.f, bot.y + 1.f }, outline_color);
															   
	ctx.m_renderer->draw_line({ bot_middle_screen, (float)ctx.m_screen_h }, { bot.x, bot.y }, snaplines_color);
}
