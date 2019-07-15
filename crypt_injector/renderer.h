#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

using vec4_d = D3DXVECTOR4;
using vec3_d = D3DXVECTOR3;
using vec2_d = D3DXVECTOR2;

class font;
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

struct font_handle
{
	font_handle() = default;
	explicit font_handle(std::size_t id);

	std::size_t id;
};

struct vertex_t
{
	vertex_t() = default;

	vertex_t(vec4_d position, D3DCOLOR color, vec2_d tex);
	vertex_t(vec4_d position, D3DCOLOR color);
	vertex_t(vec3_d position, D3DCOLOR color);
	vertex_t(vec2_d position, D3DCOLOR color);

	vertex_t(float x, float y, float z, D3DCOLOR color);
	vertex_t(float x, float y, D3DCOLOR color);

	vec4_d position;
	D3DCOLOR m_color;
	vec2_d tex{};
};

struct batch
{
	batch(std::size_t count, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr);

	std::size_t count;
	D3DPRIMITIVETYPE topology;
	IDirect3DTexture9* texture;
};

class render_list
{
public:
	render_list() = delete;
	render_list(std::size_t max_vertices);

	void clear();

	friend class Renderer;

	std::vector<vertex_t>	vertices;
	std::vector<batch>	batches;
};

class dx_color
{
	unsigned char _r = 0, _g = 0, _b = 0, _a = 0xFF;

public:
	dx_color() = default;
	dx_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) :
		_r{ r }, _g{ g }, _b{ b }, _a{ a }{}

	// gay below
	unsigned char r() const { return _r; }
	unsigned char g() const { return _g; }
	unsigned char b() const { return _b; }
	unsigned char a() const { return _a; }

	void set_r(unsigned char val) { _r = val; }
	void set_g(unsigned char val) { _g = val; }
	void set_b(unsigned char val) { _b = val; }
	void set_a(unsigned char val) { _a = val; }

	DWORD argb() const { return D3DCOLOR_ARGB(_a, _r, _g, _b); }
	DWORD rgba() const { return D3DCOLOR_ARGB(_r, _g, _b, _a); } // 200iq

	static dx_color from_hsb(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - saturation * (1.0f - f));

		if (h < 1)
		{
			return dx_color(
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255),
				(unsigned char)(p * 255)
			);
		}
		if (h < 2)
		{
			return dx_color(
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(p * 255)
			);
		}
		if (h < 3)
		{
			return dx_color(
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255),
				(unsigned char)(t * 255)
			);
		}
		if (h < 4)
		{
			return dx_color(
				(unsigned char)(p * 255),
				(unsigned char)(q * 255),
				(unsigned char)(brightness * 255)
			);
		}
		if (h < 5)
		{
			return dx_color(
				(unsigned char)(t * 255),
				(unsigned char)(p * 255),
				(unsigned char)(brightness * 255)
			);
		}
		return dx_color(
			(unsigned char)(brightness * 255),
			(unsigned char)(p * 255),
			(unsigned char)(q * 255)
		);
	}

#define def_color(name, r, g, b, a) \
	static dx_color name() { return dx_color(r, g, b, a); }

	def_color(black, 0, 0, 0, 255);
	def_color(navy, 0, 0, 128, 255);
	def_color(blue, 0, 0, 255, 255);
	def_color(green, 0, 128, 0, 255);
	def_color(teal, 0, 128, 128, 255);
	def_color(lime, 0, 255, 0, 255);
	def_color(aqua, 0, 255, 255, 255);
	def_color(maroon, 128, 0, 0, 255);
	def_color(purple, 128, 0, 128, 255);
	def_color(olive, 128, 128, 0, 255);
	def_color(gray, 128, 128, 128, 255);
	def_color(silver, 192, 192, 192, 255);
	def_color(red, 255, 0, 0, 255);
	def_color(fuchsia, 255, 0, 255, 255);
	def_color(yellow, 255, 255, 0, 255);
	def_color(white, 255, 255, 255, 255);

#undef def_color
};

class renderer
{
	IDirect3D9* m_d3d = nullptr;
	IDirect3DDevice9* m_device = nullptr;
	std::size_t m_max_vertices = 0;

	IDirect3DVertexBuffer9* vertex_buffer = nullptr;

	IDirect3DSurface9* m_backbuffer = nullptr;
	D3DSURFACE_DESC m_backbuffer_desc{};

	render_list* m_int_render_list = nullptr;
	render_list* m_render_list = nullptr;
	std::vector<font*> fonts;

public:
	renderer(std::size_t max_vertices);
	~renderer();

	render_list* make_render_list() const;
	void force_render_list(render_list* list);
	void reset_render_list();

	void start();
	void reacquire();
	void release();

	void begin() const;
	void end() const;

	void render(render_list* render_list);
	void render();

	IDirect3DDevice9* get_device() const
	{
		return m_device;
	}

	render_list* get_renderlist() const
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

	font_handle create_font(const std::string& family, long size, std::uint8_t flags = 0, int width = 0);

	template <std::size_t N>
	void add_vertices(render_list* render_list, const vertex_t(&vertex_t_array)[N], D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture = nullptr)
	{
		std::size_t num_vertices = std::size(render_list->vertices);
		if (std::empty(render_list->batches) || render_list->batches.back().topology != topology || render_list->batches.back().texture != texture)
			render_list->batches.emplace_back(0, topology, texture);

		render_list->batches.back().count += N;

		for (size_t i = 0; i < N; i++)
			render_list->vertices.emplace_back(vertex_t_array[i]);

		switch (topology)
		{
		case D3DPT_LINESTRIP:
		case D3DPT_TRIANGLESTRIP:
			render_list->batches.emplace_back(0, D3DPT_FORCE_DWORD, nullptr);
		default:
			break;
		}
	}

	void line(float x0, float y0, float x1, float y1, dx_color color);
	void line(render_list* render_list, float x0, float y0, float x1, float y1, dx_color color);

	void gradient_rect(float x, float y, float w, float h, dx_color tl, dx_color tr, dx_color bl, dx_color br);
	void gradient_rect(render_list* render_list, float x, float y, float w, float h, dx_color tl, dx_color tr, dx_color bl, dx_color br);

	void filled_rect(float x, float y, float w, float h, dx_color color);
	void filled_rect(render_list* render_list, float x, float y, float w, float h, dx_color color);

	void rect(float x, float y, float w, float h, dx_color color, float stroke_width = 1.f);
	void rect(render_list* render_list, float x, float y, float w, float h, dx_color color, float stroke_width = 1.f);

	void textured_rect(float x, float y, float w, float h, dx_color color, IDirect3DTexture9* texture, float tx = 0, float ty = 0, float tw = 0, float th = 0);
	void textured_rect(render_list* render_list, float x, float y, float w, float h, dx_color color, IDirect3DTexture9* texture, float tx = 0, float ty = 0, float tw = 0, float th = 0);

	vec2_d get_text_extent(font_handle font, const std::string& text);

	void string(render_list* render_list, font_handle font, float x, float y, const std::string& text, dx_color color, std::uint8_t flags = 0);
	void string(font_handle font, float x, float y, const std::string& text, dx_color color, std::uint8_t flags = 0);

	void circle(render_list* render_list, float x, float y, float radius, dx_color color);
	void circle(float x, float y, float radius, dx_color color);

	void arc(render_list* render_list, float x, float y, float radius, float start_angle, float end_angle, dx_color color);
	void arc(float x, float y, float radius, float start_angle, float end_angle, dx_color color);

	void filled_arc(render_list* render_list, float x, float y, float radius, float start_angle, float end_angle, dx_color color);
	void filled_arc(float x, float y, float radius, float start_angle, float end_angle, dx_color color);

	void filled_rounded_rect(render_list* render_list, float x, float y, float w, float h, dx_color color);
	void filled_rounded_rect(float x, float y, float w, float h, dx_color color);

	void rounded_rect(render_list* render_list, float x, float y, float w, float h, dx_color color);
	void rounded_rect(float x, float y, float w, float h, dx_color color);

	void triangle(float x0, float y0, float x1, float y1, float x2, float y2, dx_color color);
	void triangle(render_list* render_list, float x0, float y0, float x1, float y1, float x2, float y2, dx_color color);
};
