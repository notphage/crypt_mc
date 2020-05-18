#pragma once

class render_list_t;

enum text_flags_t : std::uint8_t
{
	TEXT_LEFT = 0 << 0,
	TEXT_CENTER = 1 << 1,
	TEXT_RIGHT = 1 << 2,
	//
	TEXT_TOP = 1 << 3,
	TEXT_MIDDLE = 1 << 4,
	TEXT_BOTTOM = 1 << 5,
	TEXT_BASELINE = 1 << 6,
	//
	TEXT_BUDGET_SHADOW = 1 << 7,
};

class c_font
{
	size_t								 m_id{ };
	std::string							 m_font{ };
	float								 m_size{ };
public:
	c_font(const std::string& font, float size);

	void draw_text(const std::unique_ptr<render_list_t>& render_list, vec2 pos, const std::string& text, color_t color, std::uint16_t flags = TEXT_CENTER);
	vec2 get_text_extent(const std::string_view& text);
	size_t max_characters_to_fit(const std::string& text, size_t max_width);
};