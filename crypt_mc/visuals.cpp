#include "context.h"
#include "visuals.h"
#include <gl/GL.h>

void c_visuals::draw_box_3d(const std::shared_ptr<c_player>& player, const vec3& pos)
{
	LI_FN(glPushMatrix).cached()();
	LI_FN(glEnable).cached()(GL_LINE_SMOOTH);

	float horz_space_frac = 1.6f;

	float minx = pos.x - (player->get_width() / 2.f * horz_space_frac);
	float maxx = minx + (player->get_width() * horz_space_frac);
	float miny = pos.y;
	float maxy = miny + player->get_height() + 0.15f;
	float minz = pos.z - (player->get_width() / 2.f * horz_space_frac);
	float maxz = minz + (player->get_width() * horz_space_frac);

	LI_FN(glTranslatef).cached()(pos.x, pos.y, pos.z);
	LI_FN(glRotatef).cached()(-player->get_yaw(), 0.f, 1.f, 0.f);
	LI_FN(glTranslatef).cached()(-pos.x, -pos.y, -pos.z);

	// Outline
	{
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	                        {
	//		                        {minx, miny, minz},
	//		                        {minx, maxy, minz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, maxy, minz},
	//		                        {maxx, miny, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {minx, miny, maxz},
	//		                        {minx, maxy, maxz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	                        {
	//		                        {maxx, maxy, minz},
	//		                        {maxx, miny, minz},
	//		                        {minx, maxy, minz},
	//		                        {minx, miny, minz},
	//		                        {minx, maxy, maxz},
	//		                        {minx, miny, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, miny, maxz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	                        {
	//		                        {minx, maxy, minz},
	//		                        {maxx, maxy, minz},
	//		                        {maxx, maxy, maxz},
	//		                        {minx, maxy, maxz},
	//		                        {minx, maxy, minz},
	//		                        {minx, maxy, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, maxy, minz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	                        {
	//		                        {minx, miny, minz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, miny, maxz},
	//		                        {minx, miny, maxz},
	//		                        {minx, miny, minz},
	//		                        {minx, miny, maxz},
	//		                        {maxx, miny, maxz},
	//		                        {maxx, miny, minz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	                        {
	//		                        {minx, miny, minz},
	//		                        {minx, maxy, minz},
	//		                        {minx, miny, maxz},
	//		                        {minx, maxy, maxz},
	//		                        {maxx, miny, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, maxy, minz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	                        {
	//		                        {minx, maxy, maxz},
	//		                        {minx, miny, maxz},
	//		                        {minx, maxy, minz},
	//		                        {minx, miny, minz},
	//		                        {maxx, maxy, minz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, miny, maxz}
	//	                        });
	}

	// Filled
	if (ctx.m_settings.visuals_esp_box_filled)
	{
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//	                        {
	//		                        {minx, miny, minz},
	//		                        {minx, maxy, minz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, maxy, minz},
	//		                        {maxx, miny, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {minx, miny, maxz},
	//		                        {minx, maxy, maxz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//	                        {
	//		                        {maxx, maxy, minz},
	//		                        {maxx, miny, minz},
	//		                        {minx, maxy, minz},
	//		                        {minx, miny, minz},
	//		                        {minx, maxy, maxz},
	//		                        {minx, miny, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, miny, maxz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//	                        {
	//		                        {minx, maxy, minz},
	//		                        {maxx, maxy, minz},
	//		                        {maxx, maxy, maxz},
	//		                        {minx, maxy, maxz},
	//		                        {minx, maxy, minz},
	//		                        {minx, maxy, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, maxy, minz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//	                        {
	//		                        {minx, miny, minz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, miny, maxz},
	//		                        {minx, miny, maxz},
	//		                        {minx, miny, minz},
	//		                        {minx, miny, maxz},
	//		                        {maxx, miny, maxz},
	//		                        {maxx, miny, minz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//	                        {
	//		                        {minx, miny, minz},
	//		                        {minx, maxy, minz},
	//		                        {minx, miny, maxz},
	//		                        {minx, maxy, maxz},
	//		                        {maxx, miny, maxz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, maxy, minz}
	//	                        });
	//
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//	                        {
	//		                        {minx, maxy, maxz},
	//		                        {minx, miny, maxz},
	//		                        {minx, maxy, minz},
	//		                        {minx, miny, minz},
	//		                        {maxx, maxy, minz},
	//		                        {maxx, miny, minz},
	//		                        {maxx, maxy, maxz},
	//		                        {maxx, miny, maxz}
	//	                        });
	}

	LI_FN(glDisable).cached()(GL_LINE_SMOOTH);
	LI_FN(glPopMatrix).cached()();
}

void c_visuals::draw_box_2d(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& player, const vec3& pos)
{
	LI_FN(glPushMatrix).cached()();
	LI_FN(glEnable).cached()(GL_LINE_SMOOTH);
	
	float horz_space_frac = 1.6f;
	
	float minx = pos.x - (player->get_width() / 2.f * horz_space_frac);
	float maxx = minx + (player->get_width() * horz_space_frac);
	float miny = pos.y;
	float maxy = miny + player->get_height() + 0.15f;
	float minz = pos.z - (player->get_width() / 2.f * horz_space_frac);
	float maxz = minz + (player->get_width() * horz_space_frac);
	
	float z = (maxz + minz) / 2.f;
	
	LI_FN(glTranslatef).cached()(pos.x, pos.y, pos.z);
	LI_FN(glRotatef).cached()(-mc->get_player_yview(), 0.f, 1.f, 0.f);
	LI_FN(glTranslatef).cached()(-pos.x, -pos.y, -pos.z);

	//ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_color, GL_LINES,
	//	{
	//		{minx, miny, z},
	//		{minx, maxy, z},
	//		{minx, maxy, z},
	//		{maxx, maxy, z},
	//		{maxx, maxy, z},
	//		{maxx, miny, z},
	//		{maxx, miny, z},
	//		{minx, miny, z}
	//	});
	
	// Filled
	if (ctx.m_settings.visuals_esp_box_filled)
	{
	//	ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_box_outline_color, GL_QUADS,
	//		{
	//			{minx, miny, z},
	//			{minx, maxy, z},
	//			{minx, maxy, z},
	//			{maxx, maxy, z},
	//			{maxx, maxy, z},
	//			{maxx, miny, z},
	//			{maxx, miny, z},
	//			{minx, miny, z}
	//		});
	}
	
	LI_FN(glDisable).cached()(GL_LINE_SMOOTH);
	LI_FN(glPopMatrix).cached()();
}

void c_visuals::draw_snap_lines(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& player, const vec3& pos)
{
	LI_FN(glPushMatrix).cached()();
	LI_FN(glEnable).cached()(GL_LINE_SMOOTH);

	mc->set_view_bobbing(false);

	//ctx.m_renderer.vertex3f(ctx.m_settings.visuals_esp_snap_lines_color, GL_LINES,
	//	{
	//		{0.f, 0.f, 0.f},
	//		{pos.x, pos.y, pos.z}
	//	});

	LI_FN(glDisable).cached()(GL_LINE_SMOOTH);
	LI_FN(glPopMatrix).cached()();
}

void c_visuals::draw_name_tags(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& player, const vec3& pos)
{
	auto color_codes = player->is_sneaking() && player->is_invisible() ? "\2475" : player->is_sneaking() ? "\247c" : player->is_invisible() ? "\247b" : "";

	auto name = player->get_name();
	const char* name_cstr = ctx.m_jni->GetStringUTFChars(name, 0);

	std::string test(name_cstr);

	ctx.m_jni->ReleaseStringUTFChars(name, name_cstr);

	jstring name_colored = ctx.m_jni->NewStringUTF(test.c_str());

	int width = (mc->get_string_width(name_colored) / 2);
	auto scale = 0.0300000024f;
	auto height = player->is_sneaking() ? player->get_height() + 0.2f : player->get_height() + 0.5f;

	LI_FN(glPushMatrix).cached()();
	LI_FN(glTranslatef).cached()(pos.x, pos.y + height, pos.z);
	LI_FN(glNormal3f).cached()(0.0f, 1.0f, 0.0f);
	LI_FN(glRotatef).cached()(-mc->get_player_yview(), 0.0f, 1.0f, 0.0f);
	LI_FN(glRotatef).cached()(mc->get_player_xview(), 1.0f, 0.0f, 0.0f);
	LI_FN(glScalef).cached()(-scale, -scale, scale);
	LI_FN(glDepthMask).cached()(false);
	LI_FN(glDisable).cached()(GL_DEPTH_TEST);
	LI_FN(glDisable).cached()(GL_LIGHTING);
	LI_FN(glEnable).cached()(GL_BLEND);
	LI_FN(glDisable).cached()(GL_TEXTURE_2D);
	LI_FN(glBegin).cached()(GL_QUADS);
	LI_FN(glColor4f).cached()(0.0f, 0.0f, 0.0f, 1.0f);
	LI_FN(glVertex3d).cached()(static_cast<double>(-width - 2), static_cast<double>(-2), 0.0);
	LI_FN(glVertex3d).cached()(static_cast<double>(-width - 2), static_cast<double>(10), 0.0);
	LI_FN(glVertex3d).cached()(static_cast<double>(width + 2), static_cast<double>(10), 0.0);
	LI_FN(glVertex3d).cached()(static_cast<double>(width + 2), static_cast<double>(-2), 0.0);
	LI_FN(glEnd).cached()();
	LI_FN(glEnable).cached()(GL_TEXTURE_2D);
	mc->draw_string_with_shadow(name_colored, -width, 0, -1);
	LI_FN(glEnable).cached()(GL_DEPTH_TEST);
	LI_FN(glDepthMask).cached()(true);
	LI_FN(glEnable).cached()(GL_LIGHTING);
	LI_FN(glDisable).cached()(GL_BLEND);
	LI_FN(glColor4f).cached()(1.0f, 1.0f, 1.0f, 1.0f);
	LI_FN(glPopMatrix).cached()();
}

void c_visuals::on_render(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	auto players = world->get_players();

	for (auto&& player : players)
	{
		if (!player || player->is_dead() || self->is_same(player))
			continue;

		const vec3 render_pos(mc->get_render_xpos(), mc->get_render_ypos(), mc->get_render_zpos());
		const vec3 origin(player->origin_x(), player->origin_y(), player->origin_z());
		const vec3 old_origin(player->old_origin_x(), player->old_origin_y(), player->old_origin_z());

		vec3 pos = old_origin + (origin - old_origin) * mc->get_render_partial_ticks() - render_pos;

		switch (ctx.m_settings.visuals_esp_box)
		{
			case 1:
			{
				draw_box_2d(mc, player, pos);
				break;
			}

			case 2:
			{
				draw_box_3d(player, pos);
				break;
			}

			case 0:
			default:
				break;
		}

		if (ctx.m_settings.visuals_esp_snap_lines)
			draw_snap_lines(mc, self, pos);

		if (ctx.m_settings.visuals_esp_nametags)
			draw_name_tags(mc, player, pos);
	}
}
