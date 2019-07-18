#pragma once
#include "renderer.h"

class c_renderer;
class render_list_t;

enum font_flags_t : std::uint8_t
{
	FONT_DEFAULT = 0 << 0,
	FONT_BOLD = 1 << 0,
	FONT_ITALIC = 1 << 1
};

enum text_flags_t : std::uint8_t
{
	TEXT_LEFT = 0 << 0,
	TEXT_RIGHT = 1 << 1,
	TEXT_CENTERED_X = 1 << 2,
	TEXT_CENTERED_Y = 1 << 3,
	TEXT_CENTERED = 1 << 2 | 1 << 3,
	TEXT_SHADOW = 1 << 4,
	TEXT_COLORTAGS = 1 << 5,
	TEXT_BUDGET_SHADOW = 1 << 6
};

class c_font
{
	void create_gdi_font(HDC ctx, HGDIOBJ* gdi_font);
	HRESULT paint_alphabet(HDC ctx, bool measure_only = false);

	IDirect3DDevice9*  m_device;
	IDirect3DTexture9* m_texture;
	long               tex_width;
	long               tex_height;
	float              text_scale;
	float              tex_coords[128 - 32][4];
	long               m_spacing;

	std::string        m_family;
	long               m_height;
	int				   m_width;
	std::uint8_t       m_flags;

public:
	c_font(IDirect3DDevice9* device, const std::string& family, long height, std::uint8_t flags = FONT_DEFAULT, int width = 0);
	void reacquire();
	void release();

	void draw_text(const std::unique_ptr<render_list_t>& render_list, vec2 pos, const std::string& text, const color_t& color, std::uint8_t flags = TEXT_LEFT);
	vec2 get_text_extent(const std::string& text);
	uint32_t max_characters_to_fit(const std::string& text, uint32_t max_width);
};