#include "context.h"
#include "renderer.h"
#include "glfontstash.h"

#include <gl/GL.h>
#pragma comment (lib, "opengl32.lib")

FONScontext* c_renderer::make_font_context() const
{
	return glfonsCreate(2048, 8192, FONS_ZERO_TOPLEFT);
}

render_list_t::ptr c_renderer::make_render_list() const
{
	return std::make_unique<render_list_t>(m_max_vertices);
}

void c_renderer::draw_scene()
{
	if (m_render_list->batches.empty())
		return;

	// Backup GL state
	LI_FN(glGetIntegerv).cached()(GL_TEXTURE_BINDING_2D, &m_last_texture);
	LI_FN(glGetIntegerv).cached()(GL_POLYGON_MODE, m_last_polygon_mode);
	LI_FN(glGetIntegerv).cached()(GL_VIEWPORT, m_last_viewport);
	LI_FN(glGetIntegerv).cached()(GL_SCISSOR_BOX, m_last_scissor_box);
	LI_FN(glPushAttrib).cached()(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);

	LI_FN(glShadeModel).cached()(GL_SMOOTH);
	LI_FN(glEnable).cached()(GL_BLEND);
	LI_FN(glBlendFunc).cached()(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	LI_FN(glDisable).cached()(GL_CULL_FACE);
	LI_FN(glDisable).cached()(GL_DEPTH_TEST);
	LI_FN(glDisable).cached()(GL_LIGHTING);
	LI_FN(glDisable).cached()(GL_COLOR_MATERIAL);
	LI_FN(glEnable).cached()(GL_SCISSOR_TEST);
	LI_FN(glEnableClientState).cached()(GL_VERTEX_ARRAY);
	LI_FN(glEnableClientState).cached()(GL_TEXTURE_COORD_ARRAY);
	LI_FN(glEnableClientState).cached()(GL_COLOR_ARRAY);
	LI_FN(glEnable).cached()(GL_TEXTURE_2D);
	LI_FN(glPolygonMode).cached()(GL_FRONT_AND_BACK, GL_FILL);

	LI_FN(glViewport).cached()(0, 0, ctx.m_screen_w, ctx.m_screen_h);
	LI_FN(glMatrixMode).cached()(GL_PROJECTION);
	LI_FN(glPushMatrix).cached()();
	LI_FN(glLoadIdentity).cached()();
	LI_FN(glOrtho).cached()(0, ctx.m_screen_w, ctx.m_screen_h, 0, -1, 1);
	LI_FN(glMatrixMode).cached()(GL_MODELVIEW);
	LI_FN(glPushMatrix).cached()();
	LI_FN(glLoadIdentity).cached()();

	LI_FN(glScissor).cached()(0, 0, ctx.m_screen_w, ctx.m_screen_h);

	auto vtx_buffer = m_render_list->vertices.data();
	LI_FN(glVertexPointer).cached()(2, GL_FLOAT, sizeof(vertex_t), (const GLvoid*)((const char*)vtx_buffer + offsetof(vertex_t, m_pos)));
	LI_FN(glColorPointer).cached()(4, GL_UNSIGNED_BYTE, sizeof(vertex_t), (const GLvoid*)((const char*)vtx_buffer + offsetof(vertex_t, m_col)));
	LI_FN(glTexCoordPointer).cached()(2, GL_FLOAT, sizeof(vertex_t), (const GLvoid*)((const char*)vtx_buffer + offsetof(vertex_t, m_tex)));

	std::size_t pos = 0;

	// Normal Shapes
	for (const auto& batch : m_render_list->batches)
	{
		int order = topology::topology_order(batch.m_topology);
		if (batch.m_count && order > 0)
		{
			if (batch.m_clip_rect == vec4{0.f, 0.f, 0.f, 0.f})
				LI_FN(glScissor).cached()(0.f, 0.f, (float)ctx.m_screen_w, float(ctx.m_screen_h));
			else
				LI_FN(glScissor).cached()(batch.m_clip_rect.x, batch.m_clip_rect.y, batch.m_clip_rect.z, batch.m_clip_rect.w);

			LI_FN(glBindTexture).cached()(GL_TEXTURE_2D, batch.m_tex_id);
			LI_FN(glDrawArrays).cached()(batch.m_topology, pos, batch.m_count);

			pos += batch.m_count;
		}
	}

	// Restore modified GL state
	LI_FN(glDisableClientState).cached()(GL_COLOR_ARRAY);
	LI_FN(glDisableClientState).cached()(GL_TEXTURE_COORD_ARRAY);
	LI_FN(glDisableClientState).cached()(GL_VERTEX_ARRAY);
	LI_FN(glBindTexture).cached()(GL_TEXTURE_2D, (GLuint)m_last_texture);
	LI_FN(glMatrixMode).cached()(GL_MODELVIEW);
	LI_FN(glPopMatrix).cached()();
	LI_FN(glMatrixMode).cached()(GL_PROJECTION);
	LI_FN(glPopMatrix).cached()();
	LI_FN(glPopAttrib).cached()();
	LI_FN(glPolygonMode).cached()(GL_FRONT, (GLenum)m_last_polygon_mode[0]);
	LI_FN(glPolygonMode).cached()(GL_BACK, (GLenum)m_last_polygon_mode[1]);
	LI_FN(glViewport).cached()(m_last_viewport[0], m_last_viewport[1], (GLsizei)m_last_viewport[2], (GLsizei)m_last_viewport[3]);
	LI_FN(glScissor).cached()(m_last_scissor_box[0], m_last_scissor_box[1], (GLsizei)m_last_scissor_box[2], (GLsizei)m_last_scissor_box[3]);

	m_render_list->clear();
}

void c_renderer::game_begin()
{
	LI_FN(glPushMatrix).cached()();

	LI_FN(glDisable).cached()(GL_TEXTURE_2D);
	LI_FN(glDisable).cached()(GL_DEPTH_TEST);
	LI_FN(glDisable).cached()(GL_CULL_FACE);
	LI_FN(glDepthMask).cached()(false);
	LI_FN(glDepthFunc).cached()(GL_LEQUAL);
	LI_FN(glEnable).cached()(GL_BLEND);
	LI_FN(glBlendFunc).cached()(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void c_renderer::game_end()
{
	LI_FN(glDepthMask).cached()(true);
	LI_FN(glDisable).cached()(GL_BLEND);
	LI_FN(glEnable).cached()(GL_TEXTURE_2D);
	LI_FN(glEnable).cached()(GL_DEPTH_TEST);
	LI_FN(glEnable).cached()(GL_CULL_FACE);

	LI_FN(glPopMatrix).cached()();
}

font_handle_t c_renderer::create_font(const std::string& family, long size)
{
	fonts.push_back(std::make_unique<c_font>(this, family, size));
	return font_handle_t{ fonts.size() - 1 };
}

void c_renderer::add_vertex(const render_list_t::ptr& render_list, vertex_t& vertex, size_t topology, size_t tex_id, const vec4& clip_rect)
{
	if (render_list->vertices.size() >= m_max_vertices)
		return;

	if (std::empty(render_list->batches) || render_list->batches.back().m_topology != topology
		|| render_list->batches.back().m_tex_id != tex_id || render_list->batches.back().m_clip_rect != clip_rect)
		render_list->batches.emplace_back(0, topology, tex_id, clip_rect);

	render_list->batches.back().m_count += 1;
	render_list->vertices.push_back(vertex);
}

void c_renderer::add_vertex(vertex_t& vertex, size_t topology, size_t tex_id, const vec4& clip_rect)
{
	add_vertex(m_render_list, vertex, topology, tex_id, clip_rect);
}

void c_renderer::draw_pixel(const vec2& pos, const color_t& color)
{
	draw_filled_rect({ pos.x, pos.y, 1.f, 1.f }, color);
}

void c_renderer::draw_pixel(const render_list_t::ptr& render_list, const vec2& pos, const color_t& color)
{
	draw_filled_rect(render_list, { pos.x, pos.y, 1.f, 1.f }, color);
}

void c_renderer::draw_line(const vec2& from, const vec2& to, const color_t& color)
{
	draw_line(m_render_list, from, to, color);
}

void c_renderer::draw_line(const render_list_t::ptr& render_list, const vec2& from, const vec2& to, const color_t& color)
{
	vertex_t v[]
	{
		{ from, color },
		{ to,   color }
	};

	add_vertices(render_list, v, GL_LINES);
}

void c_renderer::draw_gradient_rect(const vec4& rect, const color_t& tl, const color_t& tr, const color_t& bl, const color_t& br)
{
	draw_gradient_rect(m_render_list, rect, tl, tr, bl, br);
}

void c_renderer::draw_gradient_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& tl, const color_t& tr, const color_t& bl, const color_t& br)
{
	vertex_t v[] =
	{
		{ rect.x,				rect.y,			 tl },
		{ rect.x + rect.z,		rect.y,			 tr },
		{ rect.x,				rect.y + rect.w, bl },

		{ rect.x + rect.z,		rect.y,			 tr },
		{ rect.x + rect.z,		rect.y + rect.w, br },
		{ rect.x,				rect.y + rect.w, bl }
	};

	add_vertices(render_list, v, GL_TRIANGLES);
}

void c_renderer::draw_filled_rect(const vec4& rect, const color_t& color)
{
	draw_filled_rect(m_render_list, rect, color);
}

void c_renderer::draw_filled_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color)
{
	vertex_t v[]
	{
		{ rect.x,			rect.y,			 color },
		{ rect.x + rect.z,	rect.y,			 color },
		{ rect.x,			rect.y + rect.w, color },

		{ rect.x + rect.z,	rect.y,			 color },
		{ rect.x + rect.z,	rect.y + rect.w, color },
		{ rect.x,			rect.y + rect.w, color }
	};

	add_vertices(render_list, v, GL_TRIANGLES);
}

void c_renderer::draw_filled_rounded_rect(const vec4& rect, const color_t& color)
{
	draw_filled_rounded_rect(m_render_list, rect, color);
}

void c_renderer::draw_filled_rounded_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color)
{
	auto draw_corner = [&](int x, int y, int x_dir, int y_dir)
	{
		auto a = float(color.a()) / 255;

		this->draw_filled_rect(render_list, { x + x_dir * 1, y, 1, 1 }, color_t(color.r(), color.g(), color.b(), 54 * a));
		this->draw_filled_rect(render_list, { x + x_dir * 2, y, 1, 1 }, color_t(color.r(), color.g(), color.b(), 181 * a));
		this->draw_filled_rect(render_list, { x + x_dir * 3, y, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235 * a));

		this->draw_filled_rect(render_list, { x, y + y_dir * 1, 1, 1 }, color_t(color.r(), color.g(), color.b(), 54 * a));
		this->draw_filled_rect(render_list, { x, y + y_dir * 2, 1, 1 }, color_t(color.r(), color.g(), color.b(), 181 * a));
		this->draw_filled_rect(render_list, { x, y + y_dir * 3, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235 * a));

		this->draw_filled_rect(render_list, { x + x_dir * 1, y + y_dir * 1, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235 * a));
	};

	// top
	draw_corner(rect.x, rect.y, 1, 1);
	draw_corner(rect.x + rect.z - 1, rect.y, -1, 1);

	// bottom
	draw_corner(rect.x, rect.y + rect.w - 1, 1, -1);
	draw_corner(rect.x + rect.z - 1, rect.y + rect.w - 1, -1, -1);

	// center
	this->draw_filled_rect(render_list, { rect.x + 1, rect.y + 2, rect.z - 2, rect.w - 4 }, color);
	this->draw_filled_rect(render_list, { rect.x + 2, rect.y + 1, rect.z - 4, rect.w - 2 }, color);

	this->draw_filled_rect(render_list, { rect.x, rect.y + 4, rect.z, rect.w - 8 }, color);
	this->draw_filled_rect(render_list, { rect.x + 4, rect.y, rect.z - 8, rect.w }, color);
}

void c_renderer::draw_rect(const vec4& rect, const color_t& color, float stroke_width)
{
	draw_rect(m_render_list, rect, color, stroke_width);
}

void c_renderer::draw_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color, float stroke_width)
{
	vec4 tmp = rect;
	tmp.z = stroke_width;
	draw_filled_rect(render_list, tmp, color);
	tmp.x = rect.x + rect.z - stroke_width;
	draw_filled_rect(render_list, tmp, color);
	tmp.z = rect.z;
	tmp.x = rect.x;
	tmp.w = stroke_width;
	draw_filled_rect(render_list, tmp, color);
	tmp.y = rect.y + rect.w;
	draw_filled_rect(render_list, tmp, color);
}

void c_renderer::draw_rounded_rect(const vec4& rect, const color_t& color)
{
	draw_rounded_rect(m_render_list, rect, color);
}

void c_renderer::draw_rounded_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color)
{
	auto draw_corner = [&](int x, int y, int x_dir, int y_dir)
	{
		this->draw_filled_rect(render_list, { x + x_dir * 1, y, 1, 1 }, color_t(color.r(), color.g(), color.b(), 54));
		this->draw_filled_rect(render_list, { x + x_dir * 2, y, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235));
		this->draw_filled_rect(render_list, { x + x_dir * 3, y, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235));

		this->draw_filled_rect(render_list, { x, y + y_dir * 1, 1, 1 }, color_t(color.r(), color.g(), color.b(), 54));
		this->draw_filled_rect(render_list, { x, y + y_dir * 2, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235));
		this->draw_filled_rect(render_list, { x, y + y_dir * 3, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235));

		this->draw_filled_rect(render_list, { x + x_dir * 1, y + y_dir * 1, 1, 1 }, color_t(color.r(), color.g(), color.b(), 235));
	};

	// top
	draw_corner(rect.x, rect.y, 1, 1);
	draw_corner(rect.x + rect.z - 1, rect.y, -1, 1);

	// bottom
	draw_corner(rect.x, rect.y + rect.w - 1, 1, -1);
	draw_corner(rect.x + rect.z - 1, rect.y + rect.w - 1, -1, -1);

	// edges
	this->draw_filled_rect(render_list, { rect.x + 3.f, rect.y, rect.z - 6.f, 1.f }, color);
	this->draw_filled_rect(render_list, { rect.x, rect.y + 3.f, 1.f, rect.w - 6.f }, color);

	this->draw_filled_rect(render_list, { rect.x + 3.f, rect.y + rect.w - 1.f, rect.z - 6.f, 1.f }, color);
	this->draw_filled_rect(render_list, { rect.x + rect.z - 1.f, rect.y + 3.f, 1.f, rect.w - 6.f }, color);

}

void c_renderer::draw_outlined_rect(const vec4& rect, float stroke_width, const color_t& stroke_color, const color_t& fillColor)
{
	draw_outlined_rect(m_render_list, rect, stroke_width, stroke_color, fillColor);
}

void c_renderer::draw_outlined_rect(const render_list_t::ptr& render_list, const vec4& rect, float stroke_width, const color_t& stroke_color, const color_t& fillColor)
{
	draw_filled_rect(render_list, rect, fillColor);
	draw_rect(render_list, rect, stroke_color, stroke_width);
}

void c_renderer::draw_circle(const vec2& pos, float radius, const color_t& color)
{
	draw_circle(m_render_list, pos, radius, color);
}

void c_renderer::draw_circle(const render_list_t::ptr& render_list, const vec2& pos, float radius, const color_t& color)
{
	const int segments = 24;

	vertex_t v[segments + 1];

	for (int i = 0; i <= segments; i++)
	{
		float theta = 2.f * math::pi * static_cast<float>(i) / static_cast<float>(segments);

		v[i] = vertex_t{
			pos.x + radius * std::cos(theta),
			pos.y + radius * std::sin(theta),
			color
		};
	}

	add_vertices(render_list, v, GL_LINE_STRIP);
}

void c_renderer::draw_triangle(const vec2& p0, const vec2& p1, const vec2& p2, const color_t& color)
{
	draw_triangle(m_render_list, p0, p1, p2, color);
}

void c_renderer::draw_triangle(const render_list_t::ptr& render_list, const vec2& p0, const vec2& p1, const vec2& p2, const color_t& color)
{
	vertex_t v[] =
	{
		{ p0, color },
		{ p1, color },
		{ p2, color },
	};

	add_vertices(render_list, v, GL_TRIANGLES);
}

vec2 c_renderer::get_text_extent(const font_handle_t& font, const std::string& text)
{
	return fonts[font.m_id]->get_text_extent(text);
}

uint32_t c_renderer::max_characters_to_fit(const font_handle_t& font, const std::string& text, uint32_t max_width)
{
	return fonts[font.m_id]->max_characters_to_fit(text, max_width);
}

void c_renderer::string(const render_list_t::ptr& render_list, const font_handle_t& font, const vec2& pos, const std::string& text, const color_t& color, std::uint16_t flags)
{
	if (font.m_id < 0 || font.m_id >= fonts.size())
		return;

	fonts[font.m_id]->draw_text(render_list, pos, text, color, flags);
}

void c_renderer::string(const font_handle_t& font, const vec2& pos, const std::string& text, const color_t& color, std::uint16_t flags)
{
	if (flags & TEXT_BUDGET_SHADOW)
		string(m_render_list, font, { pos.x + 1, pos.y + 1 }, text, color_t::black(), flags);

	string(m_render_list, font, pos, text, color, flags);
}

batch_t::batch_t(size_t count, size_t topology, size_t tex_id = 0, const vec4& clip_rect = { 0.f, 0.f, (float)ctx.m_screen_w, (float)ctx.m_screen_h })
	: m_count(count), m_topology(topology), m_tex_id(tex_id), m_clip_rect(clip_rect)
{}

namespace topology
{
	int topology_order(size_t topology)
	{
		switch (topology)
		{
			case GL_POINTS:
				return 1;

			case GL_LINES:
			case GL_LINE_STRIP:
			case GL_LINE_LOOP:
				return 2;

			case GL_TRIANGLES:
			case GL_TRIANGLE_STRIP:
			case GL_TRIANGLE_FAN:
				return 3;

			case GL_QUADS:
			case GL_QUAD_STRIP:
				return 4;

			case GL_POLYGON:
				return 5;

			default:
				return 0;
		}
	}
}
