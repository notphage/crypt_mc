#pragma once
#include <stdint.h>
#include <memory>

#include "sdk.h"

#define GL_ARRAY_BUFFER                   0x8892
#define GL_ARRAY_BUFFER_BINDING           0x8894
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

class c_font;
#include "font.h"

namespace topology
{
	int topology_order(size_t topology);
};

struct font_handle_t
{
	font_handle_t() = default;
	explicit font_handle_t(std::size_t id)
		: m_id(id)
	{}

	std::size_t m_id;
};

struct vertex_t
{
	vertex_t() = default;

	vertex_t(float x, float y, const color_t& col = {}, const vec2& tex = {})
		: m_pos(x, y), m_col(col), m_tex(tex)
	{}

	vertex_t(const vec2& pos, const color_t& col = {}, const vec2& tex = {})
		: m_pos(pos.x, pos.y), m_col(col), m_tex(tex)
	{}

	vec2 m_pos;
	color_t m_col;
	vec2 m_tex;
};

struct batch_t
{
	batch_t(size_t count, size_t topology, size_t tex_id, const vec4& clip_rect);

	size_t m_count;
	size_t m_topology;
	size_t m_tex_id;
	vec4 m_clip_rect;
};

class render_list_t
{
	using ptr = std::unique_ptr<render_list_t>;

	friend class c_renderer;
	friend class c_font;
public:
	render_list_t() = delete;

	render_list_t(std::size_t max_verts = 0)
	{
		vertices.reserve(max_verts);
	}

	~render_list_t()
	{ }

	void clear()
	{
		vertices.clear();
		batches.clear();
	}

protected:
	std::vector<vertex_t>	vertices;
	std::vector<batch_t>	batches;
};

enum font_t
{
	font_normal,
	font_fury_title_serif,
	font_fury_header_serif,
	font_fury_normal_serif,

	font_num
};

class c_renderer : std::enable_shared_from_this<c_renderer>
{
	uint32_t m_last_active_texture = 0;
	uint32_t m_last_blend_src_rgb = 0;
	uint32_t m_last_blend_dst_rgb = 0;
	uint32_t m_last_blend_src_alpha = 0;
	uint32_t m_last_blend_dst_alpha = 0;
	uint32_t m_last_blend_equation_rgb = 0;
	uint32_t m_last_blend_equation_alpha = 0;
	int32_t m_last_program = 0;
	int32_t m_last_texture = 0;
	int32_t m_last_sampler = 0;
	int32_t m_last_array_buffer = 0;
	int32_t m_last_polygon_mode[2]{};
	int32_t m_last_viewport[4]{};
	int32_t m_last_scissor_box[4]{};
	uint8_t m_last_enable_blend = 0;
	uint8_t m_last_enable_cull_face = 0;
	uint8_t m_last_enable_depth_test = 0;
	uint8_t m_last_enable_scissor_test = 0;

	static const std::size_t m_max_vertices = 12288;

	std::vector<std::unique_ptr<c_font>> fonts;
public:
	FONScontext* m_font_context = nullptr;
	FONScontext* make_font_context() const;
	font_handle_t m_fonts[font_num];

	render_list_t::ptr m_render_list{ };
	render_list_t::ptr m_menu_element_list{ };
	render_list_t::ptr make_render_list() const;

	void draw_begin();
	void draw_scene(const render_list_t::ptr& render_list);
	void draw_end();

	font_handle_t get_font(font_t font)
	{
		return m_fonts[font];
	}

	font_handle_t create_font(const std::string& family, long size);

	void add_vertex(const render_list_t::ptr& render_list, vertex_t& vertex, size_t topology, size_t tex_id = 0, const vec4& clip_rect = { 0.f, 0.f, 0.f, 0.f });
	void add_vertex(vertex_t& vertex, size_t topology, size_t tex_id = 0, const vec4& clip_rect = { 0.f, 0.f, 0.f, 0.f });

	template <std::size_t N>
	void add_vertices(const render_list_t::ptr& render_list, vertex_t(&vertexArr)[N], size_t topology, size_t tex_id = 0, const vec4& clip_rect = { 0.f, 0.f, 0.f, 0.f })
	{
		if (render_list->vertices.size() + N >= m_max_vertices)
			return; //c_renderer::add_vertices - Vertex buffer exhausted! Increase the size of the vertex buffer or add a custom implementation.

		if (std::empty(render_list->batches) || render_list->batches.back().m_topology != topology
			|| render_list->batches.back().m_tex_id != tex_id || render_list->batches.back().m_clip_rect != clip_rect)
			render_list->batches.emplace_back(0, topology, tex_id, clip_rect);

		render_list->batches.back().m_count += N;

		render_list->vertices.resize(std::size(render_list->vertices) + N);
		std::memcpy(&render_list->vertices[std::size(render_list->vertices) - N], &vertexArr[0], N * sizeof(vertex_t));
	}

	template <std::size_t N>
	void add_vertices(vertex_t(&vertexArr)[N], size_t topology, size_t tex_id = 0, const vec4& clip_rect = { 0.f, 0.f, 0.f, 0.f })
	{
		add_vertices(m_render_list, vertexArr, topology, tex_id, clip_rect);
	}

	void draw_pixel(const vec2& pos, const color_t& color);
	void draw_pixel(const render_list_t::ptr& render_list, const vec2& pos, const color_t& color);
	void draw_line(const vec2& from, const vec2& to, const color_t& color);
	void draw_line(const render_list_t::ptr& render_list, const vec2& from, const vec2& to, const color_t& color);
	void draw_gradient_rect(const vec4& rect, const color_t& tl, const color_t& tr, const color_t& bl, const color_t& br);
	void draw_gradient_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& tl, const color_t& tr, const color_t& bl, const color_t& br);
	void draw_filled_rect(const vec4& rect, const color_t& color);
	void draw_filled_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color);
	void draw_filled_rounded_rect(const vec4& rect, const color_t& color);
	void draw_filled_rounded_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color);
	void draw_rect(const vec4& rect, const color_t& color, float stroke_width = 1.f);
	void draw_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color, float stroke_width = 1.f);
	void draw_rounded_rect(const vec4& rect, const color_t& color);
	void draw_rounded_rect(const render_list_t::ptr& render_list, const vec4& rect, const color_t& color);
	void draw_outlined_rect(const vec4& rect, float stroke_width, const color_t& stroke_color, const color_t& fill_color);
	void draw_outlined_rect(const render_list_t::ptr& render_list, const vec4& rect, float stroke_width, const color_t& stroke_color, const color_t& fill_color);
	void draw_circle(const vec2& pos, float radius, const color_t& color);
	void draw_circle(const render_list_t::ptr& render_list, const vec2& pos, float radius, const color_t& color);
	void draw_triangle(const vec2& p0, const vec2& p1, const vec2& p2, const color_t& color);
	void draw_triangle(const render_list_t::ptr& render_list, const vec2& p0, const vec2& p1, const vec2& p2, const color_t& color);

	vec2 get_text_extent(const font_handle_t& font, const std::string& text);
	uint32_t max_characters_to_fit(const font_handle_t& font, const std::string& text, uint32_t max_width);
	void string(const render_list_t::ptr& render_list, const font_handle_t& font, const vec2& pos, const std::string& text, const color_t& color, std::uint16_t flags = TEXT_LEFT);
	void string(const font_handle_t& font, const vec2& pos, const std::string& text, const color_t& color, std::uint16_t flags = TEXT_LEFT);
};
