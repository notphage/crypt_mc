#include "font.h"

#include <gl/GL.h>
#include <gl/GLU.h>

#define GL_GLEXT_PROTOTYPES
#include "glext.h"
#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")

#define FONTSTASH_IMPLEMENTATION
#include "fontstash.h"
#define GLFONTSTASH_IMPLEMENTATION
#include "glfontstash.h"

c_font::c_font(c_renderer* renderer, const std::string& font, float size)
	: m_renderer(renderer), m_font(font), m_size(size)
{
	static std::string base_path(xors("C:\\Windows\\Fonts\\"));
	static std::string extension(xors(".ttf"));
	m_id = fonsAddFont(m_renderer->m_font_context, font.c_str(), (base_path + font + extension).c_str());
}

void c_font::draw_text(const std::unique_ptr<render_list_t>& render_list, vec2 pos, const std::string& text, color_t color, std::uint16_t flags)
{
	fonsSetSize(m_renderer->m_font_context, m_size);
	fonsSetFont(m_renderer->m_font_context, m_id);
	fonsSetColor(m_renderer->m_font_context, color.rgba());
	fonsSetAlign(m_renderer->m_font_context, flags);
	
	fonsDrawText(m_renderer->m_font_context, pos.x, pos.y, text.c_str(), nullptr);
}

vec2 c_font::get_text_extent(const std::string_view& text)
{
	vec4 bounds;
	fonsSetFont(m_renderer->m_font_context, m_id);
	fonsSetSize(m_renderer->m_font_context, m_size);
	fonsTextBounds(m_renderer->m_font_context, 100, 100, text.data(), nullptr, (float*)&bounds);

	return { bounds.z - bounds.x, bounds.w - bounds.y };
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