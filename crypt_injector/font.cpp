#include "context.h"

#include "glew/glew.h"
#include "glew/wglew.h"

#include "fontstash.h"
#include "glfontstash.h"

extern FONScontext* g_font_context;

c_font::c_font(const std::string& font, float size)
	: m_font(font), m_size(size)
{
	static std::string base_path(xors("C:\\Windows\\Fonts\\"));
	static std::string extension(xors(".ttf"));
	m_id = fonsAddFont(g_font_context, font.c_str(), (base_path + font + extension).c_str());
}

void c_font::draw_text(const std::unique_ptr<render_list_t>& render_list, vec2 pos, const std::string& text, color_t color, std::uint16_t flags)
{
	fonsSetSize(g_font_context, m_size);
	fonsSetFont(g_font_context, m_id);
	fonsSetColor(g_font_context, glfonsRGBA(color.r(), color.g(), color.b(), color.a()));
	fonsSetAlign(g_font_context, flags);

	fonsDrawText(g_font_context, render_list, pos.x, pos.y + 10.f, text.c_str(), nullptr);
}

vec2 c_font::get_text_extent(const std::string_view& text)
{
	vec4 bounds;
	fonsSetFont(g_font_context, m_id);
	fonsSetSize(g_font_context, m_size);
	fonsTextBounds(g_font_context, 100, 100, text.data(), nullptr, (float*)& bounds);

	return { bounds.z - bounds.x, bounds.w - bounds.y };
}

size_t c_font::max_characters_to_fit(const std::string& text, size_t max_width)
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