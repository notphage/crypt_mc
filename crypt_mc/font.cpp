#include "context.h"

#include <gl/GL.h>
#include <gl/GLU.h>
#define GL_GLEXT_PROTOTYPES
#include "glext.h"
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

c_font::c_font(c_renderer* renderer, const std::string& family, long height, std::uint8_t flags, int width)
	: m_renderer(renderer), m_family(family), m_height(height), m_flags(flags), m_spacing(0), m_width(width)
{
	reacquire();
}

void c_font::reacquire()
{
	glGenTextures(1, &m_texture);
	if (!m_texture)
		return;

	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
	while (0xFFFF == (hr = paint_alphabet(gdi_ctx, true)))
	{
		tex_width *= 2;
		tex_height *= 2;
	}

	if (FAILED(hr))
		return;

	static constexpr long max_width = 8192;

	if (tex_width > max_width)
	{
		text_scale = static_cast<float>(max_width) / tex_width;
		tex_width = tex_height = max_width;

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
		} while (0xFFFF == (hr = paint_alphabet(gdi_ctx, true)));
	}

	DWORD* bitmap_bits = nullptr;

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

	uint32_t* pixels = new uint32_t[tex_width * tex_height];

	for (long y = 0; y < tex_height; y++)
	{
		for (long x = 0; x < tex_width; x++)
		{
			BYTE alpha = (bitmap_bits[tex_width * y + x] & 0xff);

			if (alpha > 0)
				*pixels++ = (alpha | 0xFFFFFF00);
			else
				*pixels++ = 0;
		}
	}

	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, tex_width);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, tex_width);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, tex_width, tex_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	SelectObject(gdi_ctx, prev_bitmap);
	SelectObject(gdi_ctx, prev_gdi_font);
	DeleteObject(bitmap);
	DeleteObject(gdi_font);
	DeleteDC(gdi_ctx);
}

void c_font::create_gdi_font(HDC ctx, HGDIOBJ* gdi_font)
{
	auto scaling_factor = float(GetDeviceCaps(ctx, LOGPIXELSY)) / 96.f; // for the 4k niggas

	int character_height = -MulDiv(m_height, static_cast<int>((GetDeviceCaps(ctx, LOGPIXELSY) / scaling_factor) * text_scale), 72);

	DWORD bold = (m_flags & FONT_BOLD) ? FW_BOLD : 400;
	DWORD italic = (m_flags & FONT_ITALIC) ? TRUE : FALSE;

	*gdi_font = CreateFontA(character_height, m_width, 0, 0, bold, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, m_family.c_str());
}

long c_font::paint_alphabet(HDC ctx, bool measure_only /*= false*/)
{
	SIZE size;
	char chr[2] = "x";

	if (0 == GetTextExtentPoint32A(ctx, chr, 1, &size))
		return E_FAIL;

	m_spacing = static_cast<long>(ceil(size.cy * 0.3f));

	long x = m_spacing;
	long y = 0;

	for (char c = 32; c < 127; c++)
	{
		chr[0] = c;
		if (0 == GetTextExtentPoint32A(ctx, chr, 1, &size))
			return E_FAIL;

		if (x + size.cx + m_spacing > tex_width)
		{
			x = m_spacing;
			y += size.cy + 1;
		}

		if (y + size.cy > tex_height)
			return 0xFFFF;

		if (!measure_only)
		{
			if (0 == ExtTextOutA(ctx, x + 0, y + 0, ETO_OPAQUE, nullptr, chr, 1, nullptr))
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

vec2 c_font::get_text_extent(const std::string& text)
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

void c_font::draw_text(const std::unique_ptr<render_list_t>& render_list, vec2 pos, const std::string& text, const color_t& color, std::uint8_t flags)
{
	std::size_t num_to_skip = 0;

	if (flags & (TEXT_RIGHT | TEXT_CENTERED))
	{
		vec2 size = get_text_extent(text);

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
				{ vec2{ pos.x - 0.5f,     pos.y - 0.5f + h }, color, vec2{ tx1, ty2 } },
				{ vec2{ pos.x - 0.5f,     pos.y - 0.5f     }, color, vec2{ tx1, ty1 } },
				{ vec2{ pos.x - 0.5f + w, pos.y - 0.5f + h }, color, vec2{ tx2, ty2 } },

				{ vec2{ pos.x - 0.5f + w, pos.y - 0.5f     }, color, vec2{ tx2, ty1 } },
				{ vec2{ pos.x - 0.5f + w, pos.y - 0.5f + h }, color, vec2{ tx2, ty2 } },
				{ vec2{ pos.x - 0.5f,     pos.y - 0.5f     }, color, vec2{ tx1, ty1 } }
			};

			if (flags & TEXT_SHADOW)
			{
				auto shadow_color = color_t(0, 0, 0, color.a());

				for (auto& vtx : v) { vtx.m_col = shadow_color; vtx.m_pos.x += 1.f; }
				ctx.m_renderer->add_vertices(render_list, v, GL_TRIANGLES, m_texture);

				for (auto& vtx : v) { vtx.m_pos.x -= 2.f; }
				ctx.m_renderer->add_vertices(render_list, v, GL_TRIANGLES, m_texture);

				for (auto& vtx : v) { vtx.m_pos.x += 1.f; vtx.m_pos.y += 1.f; }
				ctx.m_renderer->add_vertices(render_list, v, GL_TRIANGLES, m_texture);

				for (auto& vtx : v) { vtx.m_pos.y -= 2.f; }
				ctx.m_renderer->add_vertices(render_list, v, GL_TRIANGLES, m_texture);

				for (auto& vtx : v) { vtx.m_col = color; vtx.m_pos.y -= 1.f; }
			}

			ctx.m_renderer->add_vertices(render_list, v, GL_TRIANGLES, m_texture);
		}

		pos.x += w - (2.f * m_spacing);
	}
}

uint32_t c_font::max_characters_to_fit(const std::string& text, uint32_t max_width)
{
	size_t first, last, middle, max;

	first = 0;
	last = text.length();
	middle = (first + last) / 2;
	max = 0;

	while (first <= last && max != middle)
	{
		vec2 bounds = get_text_extent(text.substr(middle));

		if (bounds.x < max_width)
		{
			max = middle;
			first = middle + 1;
		}
		else
		{
			last = middle - 1;
		}
		middle = (first + last) / 2;
	}

	return max;
}
