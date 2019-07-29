#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class c_font;
#include "font.h"

namespace topology
{
	bool is_topology_list(D3DPRIMITIVETYPE topology);
	int topology_order(D3DPRIMITIVETYPE topology);
};

template <typename type>
void safe_release(type& com_ptr)
{
	static_assert(std::is_pointer<type>::value,
		"com_ptr is not a pointer");

	static_assert(std::is_base_of<IUnknown, std::remove_pointer<type>::type>::value,
		"com_ptr is not a pointer to a com object");

	if (com_ptr)
	{
		com_ptr->Release();
		com_ptr = nullptr;
	}
}

struct font_handle_t
{
	font_handle_t() = default;
	font_handle_t(std::size_t id)
		: m_id(id)
	{ }

	std::size_t m_id;
};

struct vertex_t
{
	vertex_t() = default;

	vertex_t(float x, float y, float z, float w, const color_t& col = {}, const vec2& tex = {})
		: m_pos(x, y, z, w), m_col(col.argb()), m_tex(tex)
	{}

	vertex_t(float x, float y, float z, const color_t& col = {}, const vec2& tex = {})
		: m_pos(x, y, z, 0.f), m_col(col.argb()), m_tex(tex)
	{}

	vertex_t(float x, float y, const color_t& col = {}, const vec2& tex = {})
		: m_pos(x, y, 0.f, 0.f), m_col(col.argb()), m_tex(tex)
	{}

	vertex_t(const vec4& pos, const color_t& col = {}, const vec2& tex = {})
		: m_pos(pos.x, pos.y, pos.z, pos.w), m_col(col.argb()), m_tex(tex)
	{}

	vertex_t(const vec3& pos, const color_t& col = {}, const vec2& tex = {})
		: m_pos(pos.x, pos.y, pos.z, 0.f), m_col(col.argb()), m_tex(tex)
	{}

	vertex_t(const vec2& pos, const color_t& col = {}, const vec2& tex = {})
		: m_pos(pos.x, pos.y, 0.f, 0.f), m_col(col.argb()), m_tex(tex)
	{}

	vec4 m_pos;
	uint32_t m_col;
	vec2 m_tex;
};

struct batch_t
{
	batch_t(size_t count, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr)
		: m_count(count), m_topology(topology), m_texture(texture)
	{ }

	size_t m_count;
	D3DPRIMITIVETYPE m_topology;
	IDirect3DTexture9* m_texture;
};

class render_list_t
{
	using ptr = std::unique_ptr<render_list_t>;

	friend class c_renderer;
	friend class c_font;

public:
	render_list_t() = delete;
	render_list_t(std::size_t max_vertices);

	void clear();

	std::vector<vertex_t> vertices;
	std::vector<batch_t> batches;
};

enum font_t
{
	font_title,
	font_normal,

	font_num
};

class c_renderer : std::enable_shared_from_this<c_renderer>
{
	IDirect3D9* m_d3d = nullptr;
	IDirect3DDevice9* m_device = nullptr;
	std::size_t m_max_vertices = 0;

	IDirect3DVertexBuffer9* vertex_buffer = nullptr;

	IDirect3DSurface9* m_backbuffer = nullptr;
	D3DSURFACE_DESC m_backbuffer_desc{};

	render_list_t::ptr m_render_list{};
	std::vector<std::unique_ptr<c_font>> fonts;
	font_handle_t m_fonts[font_num];

public:
	c_renderer(std::size_t max_vertices);
	~c_renderer();

	render_list_t::ptr make_render_list();

	void start();
	void reacquire();
	void release();

	void begin() const;
	void end() const;

	void render(const render_list_t::ptr& render_list);
	void render();

	IDirect3DDevice9* get_device() const
	{
		return m_device;
	}

	render_list_t::ptr& get_renderlist()
	{
		return m_render_list;
	}

	IDirect3DSurface9* get_backbuffer() const
	{
		return m_backbuffer;
	}

	D3DSURFACE_DESC& get_backbuffer_desc()
	{
		return m_backbuffer_desc;
	}

	font_handle_t get_font(font_t font)
	{
		return m_fonts[font];
	}

	font_handle_t create_font(const std::string& family, long size, std::uint8_t flags = 0, int width = 0);

	void add_vertex(const render_list_t::ptr& render_list, vertex_t& vertex, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr);
	void add_vertex(vertex_t& vertex, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr);

	template <std::size_t N>
	void add_vertices(const render_list_t::ptr& render_list, vertex_t(&vertexArr)[N], D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr)
	{
		if (render_list->vertices.size() + N >= m_max_vertices)
			return; //c_renderer::add_vertices - Vertex buffer exhausted! Increase the size of the vertex buffer or add a custom implementation.

		if (std::empty(render_list->batches) || render_list->batches.back().m_topology != topology
			|| render_list->batches.back().m_texture != texture)
			render_list->batches.emplace_back(0, topology, texture);

		render_list->batches.back().m_count += N;

		render_list->vertices.resize(std::size(render_list->vertices) + N);
		memcpy(&render_list->vertices[std::size(render_list->vertices) - N], &vertexArr[0], N * sizeof(vertex_t));

		switch (topology)
		{
			case D3DPT_LINESTRIP:
			case D3DPT_TRIANGLESTRIP:
				render_list->batches.emplace_back(0, D3DPT_FORCE_DWORD);

			default:
				break;
		}
	}

	template <std::size_t N>
	void add_vertices(vertex_t(&vertexArr)[N], size_t topology, IDirect3DTexture9* texture = nullptr)
	{
		add_vertices(m_render_list, vertexArr, topology, texture);
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
