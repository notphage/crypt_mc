#include "context.h"

renderer::renderer(std::size_t max_vertices) : m_max_vertices(max_vertices)
{
	m_render_list = m_int_render_list = new render_list(m_max_vertices);
}

renderer::~renderer()
{
	release();
}

render_list* renderer::make_render_list() const
{
	return new render_list(m_max_vertices);
}

void renderer::force_render_list(render_list* list)
{
	m_render_list = list;
}

void renderer::reset_render_list()
{
	m_render_list = m_int_render_list;
}

void renderer::start()
{
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof d3dpp);

	d3dpp.Windowed = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = ctx.m_window;

	m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		ctx.m_window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_device);

	reacquire();
}

void renderer::reacquire()
{
	m_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backbuffer);

	m_backbuffer->GetDesc(&m_backbuffer_desc);

	m_device->CreateVertexBuffer(m_max_vertices * sizeof(vertex_t), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertex_buffer, nullptr);
}

void renderer::release()
{
	safe_release(vertex_buffer);
	safe_release(m_backbuffer);
	safe_release(m_device);
	safe_release(m_d3d);

	for (auto font : fonts)
		font->release();

	fonts.clear();
}

void renderer::begin() const
{
	m_device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(10, 100, 255), 1.f, 0);
	m_device->BeginScene();
}

void renderer::end() const
{
	m_device->EndScene();
	m_device->Present(nullptr, nullptr, nullptr, nullptr);
}

void renderer::render(render_list* render_list)
{
	std::size_t num_vertices = std::size(render_list->vertices);
	if (num_vertices > 0)
	{
		void* data;

		if (num_vertices > m_max_vertices)
		{
			m_max_vertices = num_vertices;
			release();
			reacquire();
		}

		vertex_buffer->Lock(0, 0, &data, D3DLOCK_DISCARD);
		std::memcpy(data, std::data(render_list->vertices), sizeof(vertex_t) * num_vertices);
		vertex_buffer->Unlock();
	}

	std::size_t pos = 0;

	batch& prev_batch = render_list->batches.front();

	for (const auto& batch : render_list->batches)
	{
		int order = topology::topology_order(batch.topology);
		if (batch.count && order > 0)
		{
			std::uint32_t primitive_count = batch.count;

			if (topology::is_topology_list(batch.topology))
				primitive_count /= order;
			else
				primitive_count -= (order - 1);

			m_device->SetTexture(0, batch.texture);
			m_device->DrawPrimitive(batch.topology, pos, primitive_count);
			pos += batch.count;
		}
	}

}

void renderer::render()
{
	render(m_render_list);
	m_render_list->clear();
}

font_handle renderer::create_font(const std::string& family, long size, std::uint8_t flags, int width)
{
	fonts.push_back(new font(this, m_device, family, size, flags, width));
	return font_handle{ fonts.size() - 1 };
}

void renderer::line(render_list* render_list, float x0, float y0, float x1, float y1, dx_color color)
{
	vertex_t vertices[]
	{
		{ x0, y0, color.argb() },
		{ x1, y1,   color.argb() }
	};

	add_vertices(render_list, vertices, D3DPT_LINELIST);
}

void renderer::gradient_rect(render_list* render_list, float x, float y, float w, float h, dx_color tl, dx_color tr, dx_color bl, dx_color br)
{
	vertex_t vertices[] =
	{
		{ x,			y,     tl.argb() },
		{ x + w,		y,     tr.argb() },
		{ x,			y + h, bl.argb() },

		{ x + w,		y,     tr.argb() },
		{ x + w,		y + h, br.argb() },
		{ x,			y + h, bl.argb() }
	};

	add_vertices(render_list, vertices, D3DPT_TRIANGLELIST);
}

void renderer::filled_rect(render_list* render_list, float x, float y, float w, float h, dx_color color)
{
	gradient_rect(render_list, x, y, w, h, color, color, color, color);
}

void renderer::rect(render_list* render_list, float x, float y, float w, float h, dx_color color, float stroke_width)
{
	filled_rect(render_list, x, y, w, stroke_width, color);
	filled_rect(render_list, x, y + h - stroke_width, w, stroke_width, color);
	filled_rect(render_list, x, y, stroke_width, h, color);
	filled_rect(render_list, x + w - stroke_width, y, stroke_width, h, color);
}

void renderer::textured_rect(render_list* render_list, float x, float y, float w, float h, dx_color color, IDirect3DTexture9* texture, float tx, float ty, float tw, float th)
{
	float tx1 = 0, ty1 = 0, tx2 = 1, ty2 = 1;
	if (tx != 0 || ty != 0 || tw != 0 || th != 0)
	{
		D3DSURFACE_DESC desc{};
		texture->GetLevelDesc(0, &desc);

		tx1 = tx / desc.Width;
		ty1 = ty / desc.Height;

		tx2 = (tx + tw) / desc.Width + 0.00000001;
		ty2 = (ty + th) / desc.Height + 0.00000001;
	}

	vertex_t vertices[] =
	{
		{vec4_d{ x,		y + h,	1, 1 },	color.argb(), vec2_d{ tx1, ty2 }},
		{vec4_d{ x,		y,		1, 1 },	color.argb(), vec2_d{ tx1, ty1 }},
		{vec4_d{ x + w,	y,		1, 1 },	color.argb(), vec2_d{ tx2, ty1 }},
		{vec4_d{ x,		y + h,	1, 1 },	color.argb(), vec2_d{ tx1, ty2 }},
		{vec4_d{ x + w,	y,		1, 1 },	color.argb(), vec2_d{ tx2, ty1 }},
		{vec4_d{ x + w,	y + h,	1, 1 },	color.argb(), vec2_d{ tx2, ty2 }}
	};

	add_vertices(render_list, vertices, D3DPT_TRIANGLELIST, texture);
}

void renderer::arc(render_list* render_list, float x, float y, float radius, float start_angle, float end_angle, dx_color color)
{
	const auto deg2rad = 0.0174532925;

	float px = -1, py = -1;
	for (int i = 0; i < end_angle - start_angle; i++)
	{
		const float theta = deg2rad * (i + start_angle);

		const float cx = x + radius * std::cos(theta),
			cy = y + radius * std::sin(theta);

		if (px == -1)
		{
			px = cx;
			py = cy;
		}

		line(px, py, cx, cy, color);

		px = cx;
		py = cy;
	}
}

void renderer::filled_arc(render_list* render_list, float x, float y, float radius, float start_angle, float end_angle, dx_color color)
{
	const auto deg2rad = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095505822317253594081284811174502841027019385211055596446229489549303819644288109756659334461284756482337867831652712019091456485669234603486104543266482133936072602491412737245870066063155881748815209209628292540917153643678925903600113305305488204665213841469519415116094330572703657595919530921861173819326117931051185480744623799627495673518857527248912279381830119491298336733624406566430860213949463952247371907021798609437027705392171762931767523846748184676694051320005681271452635608277857713427 / 180.0;

	start_angle -= 90;
	end_angle -= 90;

	const int segments = 80;

	const auto start_rad = start_angle * deg2rad;
	const auto end_rad = end_angle * deg2rad;

	const float step = (end_rad - start_rad) / segments;

	vertex_t vertices[segments + 1];

	for (int i = 0; i <= segments; i++)
	{
		const float theta = start_rad + step * i;

		vertices[i] = vertex_t
		{
			x + radius * std::cos(theta),
			y + radius * std::sin(theta),
			color.argb()
		};
	}

	add_vertices(render_list, vertices, D3DPT_TRIANGLEFAN);
}

void renderer::filled_rounded_rect(render_list* render_list, float x, float y, float w, float h, dx_color color)
{
	auto draw_corner = [&](int x, int y, int x_dir, int y_dir)
	{
		auto a = float(color.a()) / 255;

		this->filled_rect(render_list, x + x_dir * 1, y, 1, 1, dx_color(color.r(), color.g(), color.b(), 54 * a));
		this->filled_rect(render_list, x + x_dir * 2, y, 1, 1, dx_color(color.r(), color.g(), color.b(), 181 * a));
		this->filled_rect(render_list, x + x_dir * 3, y, 1, 1, dx_color(color.r(), color.g(), color.b(), 235 * a));

		this->filled_rect(render_list, x, y + y_dir * 1, 1, 1, dx_color(color.r(), color.g(), color.b(), 54 * a));
		this->filled_rect(render_list, x, y + y_dir * 2, 1, 1, dx_color(color.r(), color.g(), color.b(), 181 * a));
		this->filled_rect(render_list, x, y + y_dir * 3, 1, 1, dx_color(color.r(), color.g(), color.b(), 235 * a));

		this->filled_rect(render_list, x + x_dir * 1, y + y_dir * 1, 1, 1, dx_color(color.r(), color.g(), color.b(), 235 * a));
	};

	// top
	draw_corner(x, y, 1, 1);
	draw_corner(x + w - 1, y, -1, 1);

	// bottom
	draw_corner(x, y + h - 1, 1, -1);
	draw_corner(x + w - 1, y + h - 1, -1, -1);

	// center
	this->filled_rect(render_list, x + 1, y + 2, w - 2, h - 4, color);
	this->filled_rect(render_list, x + 2, y + 1, w - 4, h - 2, color);

	this->filled_rect(render_list, x, y + 4, w, h - 8, color);
	this->filled_rect(render_list, x + 4, y, w - 8, h, color);
}

void renderer::rounded_rect(render_list* render_list, float x, float y, float w, float h, dx_color color)
{
	auto draw_corner = [&](int x, int y, int x_dir, int y_dir)
	{
		this->filled_rect(render_list, x + x_dir * 1, y, 1, 1, dx_color(color.r(), color.g(), color.b(), 54));
		this->filled_rect(render_list, x + x_dir * 2, y, 1, 1, dx_color(color.r(), color.g(), color.b(), 235));
		this->filled_rect(render_list, x + x_dir * 3, y, 1, 1, dx_color(color.r(), color.g(), color.b(), 235));

		this->filled_rect(render_list, x, y + y_dir * 1, 1, 1, dx_color(color.r(), color.g(), color.b(), 54));
		this->filled_rect(render_list, x, y + y_dir * 2, 1, 1, dx_color(color.r(), color.g(), color.b(), 235));
		this->filled_rect(render_list, x, y + y_dir * 3, 1, 1, dx_color(color.r(), color.g(), color.b(), 235));

		this->filled_rect(render_list, x + x_dir * 1, y + y_dir * 1, 1, 1, dx_color(color.r(), color.g(), color.b(), 235));
	};

	// top
	draw_corner(x, y, 1, 1);
	draw_corner(x + w - 1, y, -1, 1);

	// bottom
	draw_corner(x, y + h - 1, 1, -1);
	draw_corner(x + w - 1, y + h - 1, -1, -1);

	// edges
	this->filled_rect(render_list, x + 3, y, w - 6, 1, color);
	this->filled_rect(render_list, x, y + 3, 1, h - 6, color);

	this->filled_rect(render_list, x + 3, y + h - 1, w - 6, 1, color);
	this->filled_rect(render_list, x + w - 1, y + 3, 1, h - 6, color);
}

void renderer::triangle(render_list* render_list, float x0, float y0, float x1, float y1, float x2, float y2, dx_color color)
{
	vertex_t vertices[] =
	{
		{ x0,	y0, color.argb() },
		{ x1,	y1, color.argb() },
		{ x2,	y2, color.argb() },
	};

	add_vertices(render_list, vertices, D3DPT_TRIANGLELIST);
}

vec2_d renderer::get_text_extent(font_handle font, const std::string& text)
{
	return fonts[font.id]->get_text_extent(text);
}

void renderer::line(float x0, float y0, float x1, float y1, dx_color color)
{
	line(m_render_list, x0, y0, x1, y1, color);
}

void renderer::gradient_rect(float x, float y, float w, float h, dx_color tl, dx_color tr, dx_color bl, dx_color br)
{
	gradient_rect(m_render_list, x, y, w, h, tl, tr, bl, br);
}

void renderer::filled_rect(float x, float y, float w, float h, dx_color color)
{
	filled_rect(m_render_list, x, y, w, h, color);
}

void renderer::rect(float x, float y, float w, float h, dx_color color, float stroke_width)
{
	rect(m_render_list, x, y, w, h, color, stroke_width);
}

void renderer::textured_rect(float x, float y, float w, float h, dx_color color, IDirect3DTexture9* texture, float tx, float ty, float tw, float th)
{
	textured_rect(m_render_list, x, y, w, h, color, texture, tx, ty, tw, th);
}

void renderer::string(render_list* render_list, font_handle font, float x, float y, const std::string& text, dx_color color, std::uint8_t flags)
{
	if (font.id < 0 || font.id >= std::size(fonts))
		return;

	fonts[font.id]->draw_text(render_list, { x, y }, text, color.argb(), flags);
}

void renderer::string(font_handle font, float x, float y, const std::string& text, dx_color color, std::uint8_t flags)
{
	if (flags & TEXT_BUDGET_SHADOW)
		string(m_render_list, font, x + 1, y + 1, text, dx_color::black(), flags);

	string(m_render_list, font, x, y, text, color, flags);
}

void renderer::arc(float x, float y, float radius, float start_angle, float end_angle, dx_color color)
{
	arc(m_render_list, x, y, radius, start_angle, end_angle, color);
}

void renderer::filled_arc(float x, float y, float radius, float start_angle, float end_angle, dx_color color)
{
	filled_arc(m_render_list, x, y, radius, start_angle, end_angle, color);
}

void renderer::filled_rounded_rect(float x, float y, float w, float h, dx_color color)
{
	filled_rounded_rect(m_render_list, x, y, w, h, color);
}

void renderer::rounded_rect(float x, float y, float w, float h, dx_color color)
{
	rounded_rect(m_render_list, x, y, w, h, color);
}

void renderer::triangle(float x0, float y0, float x1, float y1, float x2, float y2, dx_color color)
{
	triangle(m_render_list, x0, y0, x1, y1, x2, y2, color);
}

render_list::render_list(std::size_t max_vertices)
{
	vertices.reserve(max_vertices);
}

void render_list::clear()
{
	vertices.clear();
	batches.clear();
}

batch::batch(std::size_t count, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture /*= nullptr*/) :
	count(count), topology(topology), texture(texture)
{
}

vertex_t::vertex_t(vec4_d position, D3DCOLOR color) : position(position), m_color(color)
{
}

vertex_t::vertex_t(vec4_d position, D3DCOLOR color, vec2_d tex) : position(position), m_color(color), tex(tex)
{
}

vertex_t::vertex_t(vec3_d position, D3DCOLOR color) : position(position, 1.f), m_color(color)
{
}

vertex_t::vertex_t(vec2_d position, D3DCOLOR color) : position(position.x, position.y, 1.f, 1.f), m_color(color)
{
}

vertex_t::vertex_t(float x, float y, float z, D3DCOLOR color) : position(x, y, z, 1.f), m_color(color)
{
}

vertex_t::vertex_t(float x, float y, D3DCOLOR color) : position(x, y, 1.f, 1.f), m_color(color)
{
}

font_handle::font_handle(std::size_t id) :
	id(id)
{
}

namespace topology
{
	bool is_topology_list(D3DPRIMITIVETYPE topology)
	{
		return topology == D3DPT_POINTLIST || topology == D3DPT_LINELIST || topology == D3DPT_TRIANGLELIST;
	}

	int topology_order(D3DPRIMITIVETYPE topology)
	{
		switch (topology)
		{
		case D3DPT_POINTLIST:
			return 1;
		case D3DPT_LINELIST:
		case D3DPT_LINESTRIP:
			return 2;
		case D3DPT_TRIANGLELIST:
		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			return 3;
		default:
			return 0;
		}
	}
};
