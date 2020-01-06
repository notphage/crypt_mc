#include "context.h"
#include "color_manager.h"

float c_color_manager::m_sinebow_progress = 0.f;

void c_color_manager::handle_colors()
{
	m_sinebow_progress += (ctx.m_frametime / 3.f);
	m_sinebow_progress = fmodf(m_sinebow_progress, 1.f);

	color_t sinebow_clr((int)(255.f * (powf(sin(M_PI * (m_sinebow_progress + 0.f / 3.f)), 2.f))),
						(int)(255.f * (powf(sin(M_PI * (m_sinebow_progress + 1.f / 3.f)), 2.f))),
						(int)(255.f * (powf(sin(M_PI * (m_sinebow_progress + 2.f / 3.f)), 2.f))));

	for (auto&& color : m_colors)
	{
		auto [mod, condition, color_rgb] = color;

		if (*condition)
		{
			switch (mod)
			{
				case sinebow:
				{
					*(color_rgb[0]) = sinebow_clr.r();
					*(color_rgb[1]) = sinebow_clr.g();
					*(color_rgb[2]) = sinebow_clr.b();
				}
			}
		}
	}
}

void c_color_manager::add_color(modifier mod, bool* condition, const std::array<uint8_t*, 3>& color_rgb)
{
	for (auto&& color : m_colors)
		if (std::get<1>(color) == condition)
			return;

	m_colors.push_back(std::make_tuple(mod, condition, color_rgb));
}
