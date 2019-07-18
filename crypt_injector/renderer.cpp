#include "context.h"

c_renderer::c_renderer(std::size_t max_vertices) : m_max_vertices(max_vertices)
{
	m_render_list = std::make_unique<render_list_t>(max_vertices);
}

c_renderer::~c_renderer()
{
	release();
}

render_list_t::ptr c_renderer::make_render_list()
{
	return std::make_unique<render_list_t>(m_max_vertices);
}

void c_renderer::start()
{
	m_d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof d3dpp);

	d3dpp.Windowed = 1;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = ctx.m_window;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;

	m_d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		ctx.m_window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&m_device);

	reacquire();
}

void c_renderer::reacquire()
{
	m_fonts[font_title] = create_font(xors("Segoe UI"), 32);
	m_fonts[font_normal] = create_font(xors("Segoe UI"), 9);

	// Setup viewport
	D3DVIEWPORT9 vp;
	vp.X = vp.Y = 0;
	vp.Width = (DWORD)ctx.m_screen_w;
	vp.Height = (DWORD)ctx.m_screen_h;
	vp.MinZ = 0.0f;
	vp.MaxZ = 1.0f;
	m_device->SetViewport(&vp);

	m_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &m_backbuffer);

	m_backbuffer->GetDesc(&m_backbuffer_desc);

	if (m_device->CreateVertexBuffer(m_max_vertices * sizeof(vertex_t), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vertex_buffer, nullptr) < 0)
		return;

	m_device->SetRenderState(D3DRS_ZENABLE, FALSE);

	m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	m_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	m_device->SetRenderState(D3DRS_ALPHAREF, 0x08);
	m_device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);

	m_device->SetRenderState(D3DRS_LIGHTING, TRUE);

	m_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	m_device->SetRenderState(D3DRS_CLIPPING, TRUE);
	m_device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
	m_device->SetRenderState(D3DRS_VERTEXBLEND, D3DVBF_DISABLE);
	m_device->SetRenderState(D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE);
	m_device->SetRenderState(D3DRS_FOGENABLE, FALSE);
	m_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	m_device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

	m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	m_device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_device->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_device->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	m_device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	m_device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	m_device->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_device->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	m_device->SetTexture(0, nullptr);
	m_device->SetStreamSource(0, vertex_buffer, 0, sizeof(vertex_t));

	m_device->SetPixelShader(nullptr);
	m_device->SetVertexShader(nullptr);

	m_device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	m_device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	m_device->SetSamplerState(0, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
	m_device->SetSamplerState(0, D3DSAMP_SRGBTEXTURE, NULL);

	// Setup orthographic projection matrix
	// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
	// Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
	{
		float L = 0.5f;
		float R = (float)ctx.m_screen_w + 0.5f;
		float T = 0.5f;
		float B = (float)ctx.m_screen_h + 0.5f;
		D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
		D3DMATRIX mat_projection =
		{ { {
			2.0f / (R - L),   0.0f,         0.0f,  0.0f,
			0.0f,         2.0f / (T - B),   0.0f,  0.0f,
			0.0f,         0.0f,         0.5f,  0.0f,
			(L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f
		} } };
		m_device->SetTransform(D3DTS_WORLD, &mat_identity);
		m_device->SetTransform(D3DTS_VIEW, &mat_identity);
		m_device->SetTransform(D3DTS_PROJECTION, &mat_projection);
	}
}

void c_renderer::release()
{
	safe_release(vertex_buffer);
	safe_release(m_backbuffer);
	safe_release(m_device);
	safe_release(m_d3d);

	for (auto&& font : fonts)
		font->release();

	fonts.clear();
}

void c_renderer::begin() const
{
	m_device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_XRGB(10, 100, 255), 1.f, 0);
	m_device->BeginScene();
}

void c_renderer::end() const
{
	m_device->EndScene();
	m_device->Present(nullptr, nullptr, nullptr, nullptr);
}

void c_renderer::render(const render_list_t::ptr& render_list)
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
	
		if (vertex_buffer->Lock(0, 0, &data, D3DLOCK_DISCARD) < 0)
			return;
	
		std::memcpy(data, std::data(render_list->vertices), num_vertices * sizeof vertex_t);
		vertex_buffer->Unlock();
	}
	
	std::size_t pos = 0;
	
	batch_t& prev_batch = render_list->batches.front();
	
	for (const auto& batch : render_list->batches)
	{
		int order = topology::topology_order(batch.m_topology);
		if (batch.m_count && order > 0)
		{
			std::uint32_t primitive_count = batch.m_count;
	
			if (topology::is_topology_list(batch.m_topology))
				primitive_count /= order;
			else
				primitive_count -= (order - 1);
	
			m_device->SetTexture(0, batch.m_texture);
			m_device->DrawPrimitive(batch.m_topology, pos, primitive_count);
			pos += batch.m_count;
		}
	}
}

void c_renderer::render()
{
	render(m_render_list);
	m_render_list->clear();
}

font_handle_t c_renderer::create_font(const std::string& family, long size, std::uint8_t flags, int width)
{
	fonts.push_back(std::make_unique<c_font>(m_device, family, size, flags, width));
	return font_handle_t{ fonts.size() - 1 };
}

void c_renderer::add_vertex(const render_list_t::ptr& render_list, vertex_t& vertex, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture)
{
	if (render_list->vertices.size() >= m_max_vertices)
		return;

	if (std::empty(render_list->batches) || render_list->batches.back().m_topology != topology
		|| render_list->batches.back().m_texture != texture)
		render_list->batches.emplace_back(0, topology, texture);

	render_list->batches.back().m_count += 1;
	render_list->vertices.push_back(vertex);
}

void c_renderer::add_vertex(vertex_t& vertex, D3DPRIMITIVETYPE topology, IDirect3DTexture9* texture)
{
	add_vertex(m_render_list, vertex, topology, texture);
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

	add_vertices(render_list, v, D3DPT_LINELIST);
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

	add_vertices(render_list, v, D3DPT_TRIANGLELIST);
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

	//m_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, v, sizeof vertex_t);

	add_vertices(render_list, v, D3DPT_TRIANGLELIST);
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
		float theta = 2.f * M_PI_F * static_cast<float>(i) / static_cast<float>(segments);

		v[i] = vertex_t{
			pos.x + radius * std::cos(theta),
			pos.y + radius * std::sin(theta),
			color
		};
	}

	add_vertices(render_list, v, D3DPT_LINELIST);
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

	add_vertices(render_list, v, D3DPT_TRIANGLELIST);
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

render_list_t::render_list_t(std::size_t max_vertices)
{
	vertices.reserve(max_vertices);
}

void render_list_t::clear()
{
	vertices.clear();
	batches.clear();
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
