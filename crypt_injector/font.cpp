#include "context.h" 

font::font(renderer* renderer, IDirect3DDevice9* device, const std::string& family, long height, std::uint8_t flags, int width)
	: m_renderer(renderer), m_device(device), m_family(family), m_height(height), m_flags(flags), m_spacing(0), m_texture(nullptr), m_width(width)
{
	reacquire();
}

void font::reacquire()
{
	HDC gdi_ctx = nullptr;

	HGDIOBJ gdi_font = nullptr;
	HGDIOBJ prev_gdi_font = nullptr;
	HBITMAP bitmap = nullptr;
	HGDIOBJ prev_bitmap = nullptr;

	text_scale = 1.0f;

	gdi_ctx = CreateCompatibleDC(nullptr);
	SetMapMode(gdi_ctx, MM_TEXT);

	create_gdi_font(gdi_ctx, &gdi_font);

	if (!gdi_font)
		return;

	prev_gdi_font = SelectObject(gdi_ctx, gdi_font);

	tex_width = tex_height = 128;

	HRESULT hr = S_OK;
	while (D3DERR_MOREDATA == (hr = paint_alphabet(gdi_ctx, true)))
	{
		tex_width *= 2;
		tex_height *= 2;
	}

	if (FAILED(hr))
		return;

	D3DCAPS9 d3dCaps;
	m_device->GetDeviceCaps(&d3dCaps);

	if (tex_width > static_cast<long>(d3dCaps.MaxTextureWidth))
	{
		text_scale = static_cast<float>(d3dCaps.MaxTextureWidth) / tex_width;
		tex_width = tex_height = d3dCaps.MaxTextureWidth;

		bool first_iteration = true;

		do
		{
			if (!first_iteration)
				text_scale *= 0.9f;

			DeleteObject(SelectObject(gdi_ctx, prev_gdi_font));

			create_gdi_font(gdi_ctx, &gdi_font);

			if (!gdi_font)
				return;

			prev_gdi_font = SelectObject(gdi_ctx, gdi_font);

			first_iteration = false;
		} while (D3DERR_MOREDATA == (hr = paint_alphabet(gdi_ctx, true)));
	}

	if (FAILED(m_device->CreateTexture(tex_width, tex_height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_texture, nullptr)))
		return;

	DWORD* bitmap_bits;

	BITMAPINFO bitmap_ctx{};
	bitmap_ctx.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_ctx.bmiHeader.biWidth = tex_width;
	bitmap_ctx.bmiHeader.biHeight = -tex_height;
	bitmap_ctx.bmiHeader.biPlanes = 1;
	bitmap_ctx.bmiHeader.biCompression = BI_RGB;
	bitmap_ctx.bmiHeader.biBitCount = 32;

	bitmap = CreateDIBSection(gdi_ctx, &bitmap_ctx, DIB_RGB_COLORS, reinterpret_cast<void**>(&bitmap_bits), nullptr, 0);

	prev_bitmap = SelectObject(gdi_ctx, bitmap);

	SetTextColor(gdi_ctx, RGB(255, 255, 255));
	SetBkColor(gdi_ctx, 0x00000000);
	SetTextAlign(gdi_ctx, TA_TOP);

	if (FAILED(paint_alphabet(gdi_ctx, false)))
		return;

	D3DLOCKED_RECT locked_rect;
	m_texture->LockRect(0, &locked_rect, nullptr, 0);

	auto dst_row = static_cast<std::uint8_t*>(locked_rect.pBits);

	for (long y = 0; y < tex_height; y++)
	{
		auto dst = reinterpret_cast<std::uint32_t*>(dst_row);
		for (long x = 0; x < tex_width; x++)
		{
			BYTE alpha = (bitmap_bits[tex_width * y + x] & 0xff);

			if (alpha > 0)
				* dst++ = ((alpha << 24) | 0x00FFFFFF);
			else
				*dst++ = 0;
		}
		dst_row += locked_rect.Pitch;
	}

	if (m_texture)
		m_texture->UnlockRect(0);

	SelectObject(gdi_ctx, prev_bitmap);
	SelectObject(gdi_ctx, prev_gdi_font);
	DeleteObject(bitmap);
	DeleteObject(gdi_font);
	DeleteDC(gdi_ctx);
}

void font::release()
{
	safe_release(m_texture);
}

void font::create_gdi_font(HDC ctx, HGDIOBJ* gdi_font)
{
	auto scaling_factor = float(GetDeviceCaps(ctx, LOGPIXELSY)) / 96.f; // for the 4k niggas

	int character_height = -MulDiv(m_height, static_cast<int>((GetDeviceCaps(ctx, LOGPIXELSY) / scaling_factor) * text_scale), 72);

	DWORD bold = (m_flags & FONT_BOLD) ? FW_BOLD : 400;
	DWORD italic = (m_flags & FONT_ITALIC) ? TRUE : FALSE;

	*gdi_font = CreateFontA(character_height, m_width, 0, 0, bold, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, m_family.c_str());
}

long font::paint_alphabet(HDC ctx, bool measure_only /*= false*/)
{
	SIZE size;
	char chr[2] = "x";

	if (0 == GetTextExtentPoint32(ctx, chr, 1, &size))
		return E_FAIL;

	m_spacing = static_cast<long>(ceil(size.cy * 0.3f));

	long x = m_spacing;
	long y = 0;

	for (char c = 32; c < 127; c++)
	{
		chr[0] = c;
		if (0 == GetTextExtentPoint32(ctx, chr, 1, &size))
			return E_FAIL;

		if (x + size.cx + m_spacing > tex_width)
		{
			x = m_spacing;
			y += size.cy + 1;
		}

		if (y + size.cy > tex_height)
			return D3DERR_MOREDATA;

		if (!measure_only)
		{
			if (0 == ExtTextOut(ctx, x + 0, y + 0, ETO_OPAQUE, nullptr, chr, 1, nullptr))
				return E_FAIL;

			tex_coords[c - 32][0] = (static_cast<float>(x + 0 - m_spacing)) / tex_width;
			tex_coords[c - 32][1] = (static_cast<float>(y + 0 + 0)) / tex_height;
			tex_coords[c - 32][2] = (static_cast<float>(x + size.cx + m_spacing)) / tex_width;
			tex_coords[c - 32][3] = (static_cast<float>(y + size.cy + 0)) / tex_height;
		}

		x += size.cx + (2 * m_spacing);
	}

	return S_OK;
}

vec2_d font::get_text_extent(const std::string& text)
{
	float row_width = 0.f;
	float row_height = (tex_coords[0][3] - tex_coords[0][1]) * tex_height;
	float width = 0.f;
	float height = row_height;

	for (const auto& c : text)
	{
		if (c == '\n')
		{
			row_width = 0.f;
			height += row_height;
		}

		if (c < ' ')
			continue;

		float tx1 = tex_coords[c - 32][0];
		float tx2 = tex_coords[c - 32][2];

		row_width += (tx2 - tx1) * tex_width - 2.f * m_spacing;

		if (row_width > width)
			width = row_width;
	}

	return { width, height };
}

void font::draw_text(render_list* render_list, vec2_d pos, const std::string& text, D3DCOLOR color, std::uint8_t flags)
{
	std::size_t num_to_skip = 0;

	if (flags & (TEXT_RIGHT | TEXT_CENTERED))
	{
		vec2_d size = get_text_extent(text);

		if (flags & TEXT_RIGHT)
			pos.x -= size.x;
		else if (flags & TEXT_CENTERED_X)
			pos.x -= 0.5f * size.x;

		if (flags & TEXT_CENTERED_Y)
			pos.y -= 0.5f * size.y;
	}

	pos.x -= m_spacing;

	float start_x = pos.x;

	for (const auto& c : text)
	{
		if (num_to_skip > 0 && num_to_skip-- > 0)
			continue;

		if (flags & TEXT_COLORTAGS && c == '{') // format: {#aarrggbb} or {##rrggbb}, {#aarrggbb} will inherit alpha from color argument.
		{
			std::size_t index = &c - &text[0];
			if (std::size(text) > index + 11)
			{
				std::string color_str = text.substr(index, 11);
				if (color_str[1] == '#')
				{
					bool alpha = false;
					if ((alpha = color_str[10] == '}') || color_str[8] == '}')
					{
						num_to_skip += alpha ? 10 : 8;
						color_str.erase(std::remove_if(std::begin(color_str), std::end(color_str), [](char c) { return !std::isalnum(c); }), std::end(color_str));
						color = std::stoul(alpha ? color_str : "ff" + color_str, nullptr, 16);
						continue;
					}
				}
			}
		}

		if (c == '\n')
		{
			pos.x = start_x;
			pos.y += (tex_coords[0][3] - tex_coords[0][1]) * tex_height;
		}

		if (c < ' ')
			continue;

		float tx1 = tex_coords[c - 32][0];
		float ty1 = tex_coords[c - 32][1];
		float tx2 = tex_coords[c - 32][2];
		float ty2 = tex_coords[c - 32][3];

		float w = (tx2 - tx1) * tex_width / text_scale;
		float h = (ty2 - ty1) * tex_height / text_scale;

		if (c != ' ')
		{
			vertex_t v[] =
			{
				{ vec4_d{ pos.x - 0.5f,     pos.y - 0.5f + h, 0.9f, 1.f }, color, vec2_d{ tx1, ty2 } },
				{ vec4_d{ pos.x - 0.5f,     pos.y - 0.5f,     0.9f, 1.f }, color, vec2_d{ tx1, ty1 } },
				{ vec4_d{ pos.x - 0.5f + w, pos.y - 0.5f + h, 0.9f, 1.f }, color, vec2_d{ tx2, ty2 } },

				{ vec4_d{ pos.x - 0.5f + w, pos.y - 0.5f,     0.9f, 1.f }, color, vec2_d{ tx2, ty1 } },
				{ vec4_d{ pos.x - 0.5f + w, pos.y - 0.5f + h, 0.9f, 1.f }, color, vec2_d{ tx2, ty2 } },
				{ vec4_d{ pos.x - 0.5f,     pos.y - 0.5f,     0.9f, 1.f }, color, vec2_d{ tx1, ty1 } }
			};

			if (flags & TEXT_SHADOW)
			{
				auto shadow_color = D3DCOLOR_ARGB((color >> 24) & 0xff, 0x00, 0x00, 0x00);

				for (auto& vtx : v) { vtx.m_color = shadow_color; vtx.position.x += 1.f; }
				m_renderer->add_vertices(render_list, v, D3DPT_TRIANGLELIST, m_texture);

				for (auto& vtx : v) { vtx.position.x -= 2.f; }
				m_renderer->add_vertices(render_list, v, D3DPT_TRIANGLELIST, m_texture);

				for (auto& vtx : v) { vtx.position.x += 1.f; vtx.position.y += 1.f; }
				m_renderer->add_vertices(render_list, v, D3DPT_TRIANGLELIST, m_texture);

				for (auto& vtx : v) { vtx.position.y -= 2.f; }
				m_renderer->add_vertices(render_list, v, D3DPT_TRIANGLELIST, m_texture);

				for (auto& vtx : v) { vtx.m_color = color; vtx.position.y -= 1.f; }
			}

			m_renderer->add_vertices(render_list, v, D3DPT_TRIANGLELIST, m_texture);
		}

		pos.x += w - (2.f * m_spacing);
	}
}
