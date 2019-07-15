#pragma once
#include "renderer.h"

class renderer;
class render_list;

enum font_flags : std::uint8_t
{
	FONT_DEFAULT = 0 << 0,
	FONT_BOLD = 1 << 0,
	FONT_ITALIC = 1 << 1
};

enum text_flags : std::uint8_t
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

class font
{
public:
	font(renderer* renderer, IDirect3DDevice9* device, const std::string& family, long height, std::uint8_t flags = FONT_DEFAULT, int width = 0);
	void reacquire();
	void release();

	void draw_text(render_list* render_list, vec2_d position, const std::string& text, D3DCOLOR color, std::uint8_t flags = TEXT_LEFT);
	vec2_d get_text_extent(const std::string& text);

private:
	void create_gdi_font(HDC ctx, HGDIOBJ* gdi_font);
	HRESULT paint_alphabet(HDC ctx, bool measure_only = false);

	IDirect3DDevice9* m_device;
	IDirect3DTexture9* m_texture;
	long                  tex_width;
	long                  tex_height;
	float                 text_scale;
	float                 tex_coords[128 - 32][4];
	long                  m_spacing;

	std::string           m_family;
	long                  m_height;
	int					  m_width;
	std::uint8_t          m_flags;

	renderer* m_renderer;
};