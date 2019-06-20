#pragma once
#include "renderer.h"
#include "fontstash.h"

class c_renderer;
class render_list_t;

enum font_flags : std::uint8_t
{
	FONT_DEFAULT = 0 << 0,
	FONT_BOLD = 1 << 0,
	FONT_ITALIC = 1 << 1
};

enum text_flags : std::uint16_t
{
	TEXT_LEFT = 1 << 0,
	TEXT_CENTERED_X = 1 << 1,
	TEXT_RIGHT = 1 << 2,
	TEXT_TOP = 1 << 3,
	TEXT_CENTERED_Y = 1 << 4,
	TEXT_BOTTOM = 1 << 5,
	TEXT_CENTERED = 1 << 4 | 1 << 1,

	TEXT_SHADOW = 1 << 7,
	TEXT_COLORTAGS = 1 << 8,
	TEXT_BUDGET_SHADOW = 1 << 9
};

class c_font
{
	size_t								 m_id{ };
	std::string							 m_font{ };
	float								 m_size{ };
	c_renderer*							 m_renderer{ };
public:
	c_font(c_renderer* renderer, const std::string& font, float size);

	void draw_text(const std::unique_ptr<render_list_t>& render_list, vec2 pos, const std::string& text, color_t color, std::uint16_t flags = TEXT_LEFT);
	vec2 get_text_extent(const std::string_view& text);
	uint32_t max_characters_to_fit(const std::string& text, uint32_t max_width);
};
