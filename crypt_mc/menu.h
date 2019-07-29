#pragma once
#include "context.h"

#define UTL_FUNCTION( x,y,w,h ) [&]( const float& x, const float& y, const float& w, const float& h )
#define UTL_UNUSED true

namespace UI
{
	//void string(color_t, float, float, bool, const char*, ...)
	//{
	//
	//}
	//
	//uint32_t text_width(const char*, ...)
	//{
	//	return 0;
	//}
	
	class c_cursor
	{
		int cursor_x{}, cursor_y{};
		float hue_cycle{};
	public:
		void handle()
		{
			g_input.get_cursor_pos(cursor_x, cursor_y);

			hue_cycle = math::clamp<float>(hue_cycle + 0.002f, 0.f, 1.f);

			draw();
		}

		void draw()
		{
			const color_t outer_color(3, 6, 26, 215);
			const color_t inner_color(255, 255, 255, 255); //color_t::FromHSB( hue_cycle, 1.0f, 1.0f )

#pragma warning( push )
#pragma warning( disable : 4244 )

			ctx.m_renderer->draw_filled_rect({ cursor_x + 1, cursor_y, 1, 17 }, outer_color);
			
			for (int i = 0; i < 11; i++)
				ctx.m_renderer->draw_filled_rect({ cursor_x + 2 + i, cursor_y + 1 + i, 1, 1 }, outer_color);
			
			ctx.m_renderer->draw_filled_rect({ cursor_x + 8, cursor_y + 12, 5, 1 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 8, cursor_y + 13, 1, 1 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 9, cursor_y + 14, 1, 2 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 10, cursor_y + 16, 1, 2 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 8, cursor_y + 18, 2, 1 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 7, cursor_y + 16, 1, 2 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 6, cursor_y + 14, 1, 2 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 5, cursor_y + 13, 1, 1 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 4, cursor_y + 14, 1, 1 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 3, cursor_y + 15, 1, 1 }, outer_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 2, cursor_y + 16, 1, 1 }, outer_color);
			
			for (int i = 0; i < 4; i++)
				ctx.m_renderer->draw_filled_rect({ cursor_x + 2 + i, cursor_y + 2 + i, 1, 14 - (i * 2) }, inner_color);
			
			ctx.m_renderer->draw_filled_rect({ cursor_x + 6, cursor_y + 6, 1, 8 }, inner_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 7, cursor_y + 7, 1, 9 }, inner_color);
			
			for (int i = 0; i < 4; i++)
				ctx.m_renderer->draw_filled_rect({ cursor_x + 8 + i, cursor_y + 8 + i, 1, 4 - i }, inner_color);
			
			ctx.m_renderer->draw_filled_rect({ cursor_x + 8, cursor_y + 14, 1, 4 }, inner_color);
			ctx.m_renderer->draw_filled_rect({ cursor_x + 9, cursor_y + 16, 1, 2 }, inner_color);

#pragma warning( pop )
		}
	};

	class c_control
	{
	public:
		vec2 m_start;
		vec2 m_bounds;
		vec2 m_end;

		int m_cursor_x;
		int m_cursor_y;
		float m_drag_x;
		float m_drag_y;

		float m_alpha = 255;

		bool m_dragging;
		bool m_should_draw = true;
		bool m_is_inside;

		size_t m_current_tab;

		__forceinline bool is_inside(const float& x, const float& y, const float& w, const float& h)
		{
			return m_cursor_x > x && m_cursor_x < x + w && m_cursor_y > y && m_cursor_y < y + h + 1.0f;
		}

		__forceinline bool __is_inside(const float& x, const float& y, const float& w, const float& h)
		{
			return m_cursor_x > x && m_cursor_x < w && m_cursor_y > y && m_cursor_y < h + 1.0f;
		}
	};

	class c_window_data
	{
	public:
		int m_draw_counter{};

		bool m_first_click{};
		bool m_left_click{};
		bool m_right_click{};
		bool m_ignore{};

		char m_keyboard_input{};

		float m_x{};
		float m_y{};
		float m_fade_speed;
		color_t m_color = color_t(181, 0, 41, 255);
		int m_alpha = 0;
		float m_animation_progress = 0.f;
		bool m_active = true;
	};

	class c_utl_wnd : public c_control
	{
	public:
		static constexpr bool m_should_nigga = ((((((((((((((((((((((bool)((uint32_t)((int)((float)(1)))))))))))))))))))))))));
		bool m_should_draw;
		bool m_should_i_jump_off_a_bridge;

		vec2 m_window_size{};
		vec2* m_pos_setting;
		c_window_data m_data{};
		const float m_edge_size = 8.f;
		bool m_is_inside_edge{};

		void draw_box(float x, float y, float w, float h) const
		{
			// draw black background
			ctx.m_renderer->draw_filled_rect({ x, y, w, h }, color_t(1, 1, 1, m_data.m_alpha));

			// draw grey edge
			ctx.m_renderer->draw_filled_rect({ x + 1.f, y + 1.f, w - 2.f, h - 2.f }, color_t(44, 44, 44, m_data.m_alpha));

			// draw inner box
			ctx.m_renderer->draw_filled_rect({ x + 2.f, y + 2.f, w - 4.f, h - 4.f }, color_t(9, 9, 9, m_data.m_alpha));
		};

	public:
		c_utl_wnd(vec2* pos_setting, float w = 482.f, float h = 550.f)
		{
			m_pos_setting = pos_setting;
			m_window_size = vec2(w, h);
			m_bounds = vec2(w + 20.f, h + 45.f);
			m_end = (*m_pos_setting) + m_bounds;
		}

		__forceinline void resize(const float& w, const float& h) { }

		__forceinline void resize(const vec2& bounds) { }

		__forceinline void resize(const float& h)
		{
			m_window_size.y = h;
			m_bounds.y = h + 45.f;
			m_end = (*m_pos_setting) + m_bounds;
		}

		__forceinline void update_size()
		{
			m_end = (*m_pos_setting) + m_bounds;
		}

		bool handle(const std::string& tab, const bool& setting, const std::function<void(const float&, const float&, const float&, const float&)>& feature = nullptr, const bool& close_with_menu = false)
		{
			m_data.m_draw_counter++;
			m_data.m_keyboard_input = 0; //g_input.g()et_keyboard_input( );

			if (!m_should_i_jump_off_a_bridge)
				m_should_draw = setting;

			if (close_with_menu && setting && !m_should_i_jump_off_a_bridge)
			{
				if (ctx.m_menu_closing)
					m_should_draw = false;

				if (ctx.m_menu_opening)
					m_should_draw = true;
			}

			m_should_i_jump_off_a_bridge = false;

			if (ctx.m_settings.gui_fade_speed > 0)
				m_data.m_alpha = math::clamp<int>(m_should_draw ? (m_data.m_alpha + ctx.m_settings.gui_fade_speed) : (m_data.m_alpha - ctx.m_settings.gui_fade_speed), 0, 255);
			else
				m_data.m_alpha = math::clamp<int>(m_should_draw ? (m_data.m_alpha + 255) : (m_data.m_alpha - 255), 0, 255); // sorry

			m_data.m_color = color_t(ctx.m_settings.gui_accent_color().r(), ctx.m_settings.gui_accent_color().g(), ctx.m_settings.gui_accent_color().b());
			*m_data.m_color.a_ptr() = m_data.m_alpha;

			if (m_data.m_alpha < 0.1f)
			{
				if (close_with_menu && feature)
					feature(m_pos_setting->x + 15.0f, m_pos_setting->y + 40.0f, m_window_size.x, m_window_size.y);

				return false;
			}

			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_is_inside = m_cursor_x > m_pos_setting->x && m_cursor_x < m_pos_setting->x + m_bounds.x && m_cursor_y > m_pos_setting->y && m_cursor_y < m_pos_setting->y + m_bounds.y;
			m_is_inside_edge = m_is_inside && ((m_cursor_x > m_pos_setting->x + m_bounds.x - m_edge_size || m_cursor_x < m_pos_setting->x + m_edge_size) || (m_cursor_y > m_pos_setting->y + m_bounds.y - m_edge_size || m_cursor_y < m_pos_setting->y + (30.0f + m_edge_size)));

			// update data
			m_data.m_first_click = !m_data.m_left_click && g_input.is_key_pressed(KEYS_MOUSE1);
			m_data.m_left_click = g_input.is_key_pressed(KEYS_MOUSE1);
			m_data.m_right_click = g_input.is_key_pressed(KEYS_MOUSE2);

			if (!m_data.m_left_click)
				m_data.m_ignore = false;

			if (m_dragging && !m_data.m_left_click)
				m_dragging = false;

			if (m_data.m_first_click && m_is_inside_edge)
			{
				m_dragging = true;
				m_drag_x = m_cursor_x - m_pos_setting->x;
				m_drag_y = m_cursor_y - m_pos_setting->y;
			}

			if (m_dragging)
			{
				m_pos_setting->x = math::clamp(static_cast<float>(m_cursor_x - m_drag_x), 0.0f, static_cast<float>(ctx.m_screen_w) - m_bounds.x);
				m_pos_setting->y = math::clamp(static_cast<float>(m_cursor_y - m_drag_y), 0.0f, static_cast<float>(ctx.m_screen_h) - m_bounds.y);
			}

			m_end = *m_pos_setting + m_bounds;

			// draw black background
			ctx.m_renderer->draw_rect({ m_pos_setting->x, m_pos_setting->y, m_bounds.x, m_bounds.y }, color_t(1, 1, 1, m_data.m_alpha));

			// draw outer light grey box
			ctx.m_renderer->draw_rect({ m_pos_setting->x + 1.f, m_pos_setting->y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, color_t(57, 57, 57, m_data.m_alpha));

			// draw dark grey box
			ctx.m_renderer->draw_filled_rect({ m_pos_setting->x + 2.f, m_pos_setting->y + 2.f, m_bounds.x - 4.f, m_bounds.y - 4.f }, color_t(35, 35, 35, m_data.m_alpha * 0.5f));

			// draw inner light grey box
			ctx.m_renderer->draw_rect({ m_pos_setting->x + 5.f, m_pos_setting->y + 5.f, m_bounds.x - 10.f, m_bounds.y - 10.f }, color_t(57, 57, 57, m_data.m_alpha));

			// draw main menu background
			ctx.m_renderer->draw_filled_rect({ m_pos_setting->x + 6.f, m_pos_setting->y + 6.f, m_bounds.x - 12.f, m_bounds.y - 12.f }, color_t(4, 4, 4, m_data.m_alpha * 0.5f));

			// draw main box
			draw_box(m_pos_setting->x + 10.f, m_pos_setting->y + 35.f, m_window_size.x, m_window_size.y);

			// tabz
			const color_t tab_color_shadow(0, 0, 0, (m_data.m_alpha / 2) / 2);
			const color_t tab_color_selected = m_data.m_color;

			// spacing per tab
			float tab_spacing = 3.f;

			// total used by spacing
			float spacing = tab_spacing;

			// tab width
			float tab_width = m_window_size.x - spacing;

			float tab_x = m_pos_setting->x + 10.f + (tab_width + tab_spacing) * 0;
			float tab_y = m_pos_setting->y + 25.f;

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { (tab_x + (tab_width * 0.5f)) + 1.f, (tab_y - 14.f) + 1.f }, tab, color_t(0, 0, 0, m_data.m_alpha));
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { tab_x + (tab_width * 0.5f), tab_y - 14.f }, tab, color_t(206, 206, 206, m_data.m_alpha));

			ctx.m_renderer->draw_filled_rect({ tab_x, tab_y, tab_width, 4.f }, tab_color_selected);
			ctx.m_renderer->draw_filled_rect({ tab_x, tab_y + 2.f, tab_width, 4.f }, tab_color_shadow);

			// always pass feature, , 
			if (feature)
				feature(m_pos_setting->x + 15.0f, m_pos_setting->y + 40.0f, m_window_size.x, m_window_size.y);

			return true;
		}

		void column_left()
		{
			m_data.m_x = m_pos_setting->x + 50.f;
			m_data.m_y = m_pos_setting->y + 60.f;
		}

		void column_right()
		{
			m_data.m_x = m_pos_setting->x + 280.f;
			m_data.m_y = m_pos_setting->y + 60.f;
		}

		c_window_data* data()
		{
			return &m_data;
		}
	};

	class c_window : public c_control
	{
		bool m_is_open{};
		bool m_was_open{};
		bool m_should_open = true;

		vec2 m_window_size{};
		vec2* m_pos_setting;
		c_window_data m_data{};
		const float m_edge_size = 8.f;
		bool m_is_inside_edge{};

		void draw_box(float x, float y, float w, float h) const
		{
			// draw black background
			ctx.m_renderer->draw_filled_rect({ x, y, w, h }, color_t(1, 1, 1, m_data.m_alpha));

			// draw grey edge
			ctx.m_renderer->draw_filled_rect({ x + 1.f, y + 1.f, w - 2.f, h - 2.f }, color_t(44, 44, 44, m_data.m_alpha));

			// draw inner box
			ctx.m_renderer->draw_filled_rect({ x + 2.f, y + 2.f, w - 4.f, h - 4.f }, color_t(9, 9, 9, m_data.m_alpha));
		};

		void draw_watermark(float x, float y, float w, float h)
		{
			const color_t text_shadow(0, 0, 0, m_data.m_alpha);
			const color_t text_color(206, 206, 206, m_data.m_alpha);

			std::string time = xors(__DATE__);
			std::string watermark = time + xors(" | ") + ctx.m_username;

			float text_w = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), watermark).x;

			float text_x = x + w - text_w - 5.f;
			float text_y = y + h - 16.f;

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { text_x + 1.f, text_y + 1.f }, watermark, text_shadow);
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { text_x, text_y }, watermark, text_color);
		};

	public:
		c_window(vec2* pos_setting, float w = 482.f, float h = 550.f)
		{
			// sorry lol
			m_pos_setting = pos_setting;
			m_start = *m_pos_setting;
			m_window_size = vec2(w, h);
			m_bounds = vec2(w + 40.f, h + 60.f);
			m_end = m_start + m_bounds;
		}

		bool is_open() const
		{
			return m_is_open;
		}

		bool handle(std::vector<std::string>& tabs, color_t color)
		{
			ctx.m_block_keyinput = false;

			m_data.m_draw_counter++;
			m_data.m_keyboard_input = g_input.get_keyboard_input();

			if (g_input.is_key_pressed(ctx.m_settings.menu_key().key))
			{
				if (!m_was_open)
					m_should_open = !m_should_open;

				m_was_open = true;
			}
			else
				m_was_open = false;

			if (ctx.m_settings.gui_fade_speed > 0)
				m_data.m_alpha = math::clamp<int>(m_should_open ? (m_data.m_alpha + ctx.m_settings.gui_fade_speed) : (m_data.m_alpha - ctx.m_settings.gui_fade_speed), 0, 255);
			else
				m_data.m_alpha = math::clamp<int>(m_should_open ? (m_data.m_alpha + 255) : (m_data.m_alpha - 255), 0, 255); // sorry

			ctx.m_menu_closing = m_data.m_alpha < 255 && m_data.m_alpha > 1;
			ctx.m_menu_opening = m_should_open && m_data.m_alpha < 255;

			if (ctx.m_settings.gui_rainbow)
				ctx.m_settings.gui_accent_color() = ctx.m_settings.gui_accent_color();/* (m_data.m_color = UI::color_t::get_rainbow()); */
			else
				m_data.m_color = color;

			*m_data.m_color.a_ptr() = m_data.m_alpha;

			m_is_open = m_data.m_alpha > 0;
			if (!m_is_open)
				return false;

			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + m_bounds.x && m_cursor_y > m_start.y && m_cursor_y < m_start.y + m_bounds.y;
			m_is_inside_edge = m_is_inside && ((m_cursor_x > m_start.x + m_bounds.x - m_edge_size || m_cursor_x < m_start.x + m_edge_size) || (m_cursor_y > m_start.y + m_bounds.y - m_edge_size || m_cursor_y < m_start.y + m_edge_size));

			// update data
			m_data.m_first_click = !m_data.m_left_click && g_input.is_key_pressed(KEYS_MOUSE1);
			m_data.m_left_click = g_input.is_key_pressed(KEYS_MOUSE1);
			m_data.m_right_click = g_input.is_key_pressed(KEYS_MOUSE2);
			m_data.m_animation_progress = math::clamp<float>(m_data.m_animation_progress + (ctx.m_frametime * 15.f), 0.f, 1.f);

			if (!m_data.m_left_click)
				m_data.m_ignore = false;

			if (m_dragging && !m_data.m_left_click)
				m_dragging = false;

			if (m_data.m_first_click && m_is_inside_edge)
			{
				m_dragging = true;
				m_drag_x = m_cursor_x - m_start.x;
				m_drag_y = m_cursor_y - m_start.y;
			}

			if (m_dragging)
			{
				m_start.x = math::clamp(static_cast<float>(m_cursor_x - m_drag_x), 0.0f, static_cast<float>(ctx.m_screen_w) - m_bounds.x);
				m_start.y = math::clamp(static_cast<float>(m_cursor_y - m_drag_y), 0.0f, static_cast<float>(ctx.m_screen_h) - m_bounds.y);

				*m_pos_setting = m_start;
			}

			m_end = *m_pos_setting + m_bounds;

			// draw black background
			ctx.m_renderer->draw_rect({ m_start.x, m_start.y, m_bounds.x, m_bounds.y }, color_t(1, 1, 1, m_data.m_alpha));

			// draw outer light grey box
			ctx.m_renderer->draw_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, color_t(57, 57, 57, m_data.m_alpha));

			// draw dark grey box
			ctx.m_renderer->draw_filled_rect({ m_start.x + 2.f, m_start.y + 2.f, m_bounds.x - 4.f, m_bounds.y - 4.f }, color_t(35, 35, 35, m_data.m_alpha * 0.5f));

			// draw inner light grey box
			ctx.m_renderer->draw_rect({ m_start.x + 5.f, m_start.y + 5.f, m_bounds.x - 10.f, m_bounds.y - 10.f }, color_t(57, 57, 57, m_data.m_alpha));

			// draw main menu background
			ctx.m_renderer->draw_filled_rect({ m_start.x + 6.f, m_start.y + 6.f, m_bounds.x - 12.f, m_bounds.y - 12.f }, color_t(4, 4, 4, m_data.m_alpha * 0.5f));

			// draw main box
			draw_box(m_start.x + 20.f, m_start.y + 40.f, m_window_size.x, m_window_size.y);

			// draw date/username
			draw_watermark(m_start.x + 20.f, m_start.y + 40.f, m_window_size.x, m_window_size.y);

			// tabz
			const color_t text_color(153, 153, 153, m_data.m_alpha);
			const color_t text_color_shadow(0, 0, 0, m_data.m_alpha);

			const color_t tab_color(36, 36, 36, m_data.m_alpha);
			const color_t tab_color_shadow(0, 0, 0, m_data.m_alpha / 2);
			const color_t tab_color_selected = m_data.m_color;
			color_t tab_color_hovered = tab_color_selected;
			*tab_color_hovered.a_ptr() /= 2;

			// spacing per tab
			float tab_spacing = 3.f;

			// total used by spacing
			float spacing = (tabs.size()) * tab_spacing;

			// tab width
			float tab_width = (m_window_size.x - spacing) / tabs.size();

			for (size_t i = 0; i < tabs.size(); i++)
			{
				float tab_x = m_start.x + 20.f + (tab_width + tab_spacing) * i;
				float tab_y = m_start.y + 30.f;

				float text_x = tab_x + (tab_width * 0.5f);
				float text_y = tab_y - 14.f;

				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { text_x + 1.f, text_y + 1.f }, tabs.at(i), text_color_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { text_x, text_y }, tabs.at(i), text_color);

				ctx.m_renderer->draw_filled_rect({ tab_x, tab_y, tab_width, 4.f }, m_current_tab == i ? tab_color_selected : tab_color);
				ctx.m_renderer->draw_filled_rect({ tab_x, tab_y + 2.f, tab_width, 4.f }, tab_color_shadow);

				if (is_inside(tab_x, tab_y - 16.f, tab_width, 20.f) && m_current_tab != i)
				{
					//select it if we are inside it and pressing
					if (m_data.m_first_click)
					{
						m_current_tab = i;
						m_data.m_animation_progress = 0.f;
					}
					else
					{
						ctx.m_renderer->draw_filled_rect({ tab_x, tab_y, tab_width, 4.f }, tab_color_hovered);
						ctx.m_renderer->draw_filled_rect({ tab_x, tab_y + 2.f, tab_width, 4.f }, tab_color_shadow);
					}
				}
			}

			return true;
		}

		void column_left()
		{
			m_data.m_x = m_start.x + 50.f;
			m_data.m_y = m_start.y + 60.f;
		}

		void column_right()
		{
			m_data.m_x = m_start.x + 280.f;
			m_data.m_y = m_start.y + 60.f;
		}

		c_window_data* data()
		{
			return &m_data;
		}
	};

	class c_groupbox : public c_control
	{
		static constexpr size_t m_text_length = 32;

		char m_text[m_text_length]{};

	public:
		void start(c_window_data* data, const char* text)
		{
			strncpy_s(m_text, m_text_length, text, m_text_length);
			m_start = vec2(data->m_x - 10.f, data->m_y + 5.f);

			// setup coord for next item
			data->m_y += 15.f;
		}

		void end(c_window_data* data)
		{
			m_end = vec2(data->m_x - 10.f, data->m_y + 5.f);

			// setup coord for next item
			data->m_y += 15.f;

			draw(data);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t shadow_color(0, 0, 0, data->m_alpha);
			color_t edge_color(44, 44, 44, data->m_alpha);

			float width = 211.f;

			float text_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), m_text);

			// top
			ctx.m_renderer->draw_line({ m_start.x, m_start.y }, { m_start.x + 8.f, m_start.y }, edge_color);
			ctx.m_renderer->draw_line({ m_start.x + 12.f + text_width }, { m_start.y, m_start.x + width, m_start.y }, edge_color);
			// left
			ctx.m_renderer->draw_line({ m_start.x, m_start.y }, { m_end.x, m_end.y }, edge_color);
			// right
			ctx.m_renderer->draw_line({ m_start.x + width, m_start.y }, { m_end.x + width, m_end.y + 1.f }, edge_color);
			ctx.m_renderer->draw_line({ m_start.x + width + 1.f, m_start.y }, { m_end.x + width + 1.f, m_end.y + 1.f }, shadow_color);
			// bottom
			ctx.m_renderer->draw_line({ m_end.x, m_end.y }, { m_end.x + width + 1.f, m_end.y }, edge_color);
			ctx.m_renderer->draw_line({ m_end.x, m_end.y + 1.f }, { m_end.x + width + 2.f, m_end.y + 1.f }, shadow_color);

			if (strlen(m_text) > 0)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 11.f, m_start.y - 5.f }, m_text, shadow_color);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 10.f, m_start.y - 6.f }, m_text, text_color);

				// dont leave it around in memory unencrypted
				memset(m_text, 0, m_text_length);
			}
		}
	};

	class c_label : public c_control
	{
		const char* m_text{};
	public:
		c_label() { }

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 21.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 20.f, m_start.y - 3.f }, m_text, text_color);
			}
		}

		void handle(c_window_data* data, const char* text)
		{
			m_text = text;
			m_start = vec2(data->m_x, data->m_y);

			// setup coord for next item
			data->m_y += 15.f;

			draw(data);
		}
	};

	class c_checkbox : public c_control
	{
		const char* m_text{};
		bool* m_value{};
	public:
		c_checkbox()
		{
			m_bounds = vec2(8, 8);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 21.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 20.f, m_start.y - 3.f }, m_text, text_color);
			}

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_start.x, m_start.y, m_bounds.x, m_bounds.y }, color_t(0, 0, 0, data->m_alpha));

			// inner box empty
			color_t empty_color_top = color_t(69, 69, 69, data->m_alpha);
			color_t empty_color_bottom
			(
				empty_color_top.r() > 25 ? empty_color_top.r() - 25 : 0,
				empty_color_top.g() > 25 ? empty_color_top.g() - 25 : 0,
				empty_color_top.b() > 25 ? empty_color_top.b() - 25 : 0,
				data->m_alpha
			);

			ctx.m_renderer->draw_gradient_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, empty_color_top, empty_color_top, empty_color_bottom, empty_color_bottom);

			// inner box
			if (*m_value)
			{
				int anim_alpha = (float(data->m_alpha) * data->m_animation_progress);

				color_t checkbox_color_top = data->m_color;
				color_t checkbox_color_bottom
				(
					checkbox_color_top.r() > 25 ? checkbox_color_top.r() - 25 : 0,
					checkbox_color_top.g() > 25 ? checkbox_color_top.g() - 25 : 0,
					checkbox_color_top.b() > 25 ? checkbox_color_top.b() - 25 : 0,
					anim_alpha
				);

				ctx.m_renderer->draw_gradient_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, checkbox_color_top, checkbox_color_top, checkbox_color_bottom, checkbox_color_bottom);
			}
		}

		void handle(c_window_data* data, const char* name, bool* setting)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = name;
			m_value = setting;

			m_start = vec2(data->m_x, data->m_y);
			m_end = m_start + m_bounds;

			// setup coord for next item
			data->m_y += 15.f;

			float text_width = m_text ? ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), m_text).x : 0.f;

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + 20.f + text_width + 5.f && m_cursor_y > m_start.y - 3.f && m_cursor_y < m_end.y + 3.f;

			if (m_value && data->m_left_click && !data->m_ignore && m_is_inside)
			{
				*m_value = !*m_value;
				data->m_ignore = true;
			}

			draw(data);
		}
	};

	class c_checkbox_smoll : public c_control
	{
		bool* m_value{};
	public:
		c_checkbox_smoll()
		{
			m_bounds = vec2(8, 8);
		}

		void draw(c_window_data* data)
		{
			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_start.x, m_start.y, m_start.x + m_bounds.x, m_start.y + m_bounds.y }, color_t(0, 0, 0, data->m_alpha));

			// inner box empty
			color_t empty_color_top = color_t(69, 69, 69, data->m_alpha);
			color_t empty_color_bottom
			(
				empty_color_top.r() > 25 ? empty_color_top.r() - 25 : 0,
				empty_color_top.g() > 25 ? empty_color_top.g() - 25 : 0,
				empty_color_top.b() > 25 ? empty_color_top.b() - 25 : 0,
				data->m_alpha
			);

			ctx.m_renderer->draw_gradient_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, empty_color_top, empty_color_top, empty_color_bottom, empty_color_bottom);

			// inner box
			if (*m_value)
			{
				int anim_alpha = (float(data->m_alpha) * data->m_animation_progress);

				color_t checkbox_color_top = data->m_color;
				color_t checkbox_color_bottom
				(
					checkbox_color_top.r() > 25 ? checkbox_color_top.r() - 25 : 0,
					checkbox_color_top.g() > 25 ? checkbox_color_top.g() - 25 : 0,
					checkbox_color_top.b() > 25 ? checkbox_color_top.b() - 25 : 0,
					anim_alpha
				);

				ctx.m_renderer->draw_gradient_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, checkbox_color_top, checkbox_color_top, checkbox_color_bottom, checkbox_color_bottom);
			}
		}

		void handle(c_window_data* data, bool* setting)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_value = setting;

			m_start = vec2(data->m_x, data->m_y);
			m_end = m_start + m_bounds;

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + 20.f + 100.f && m_cursor_y > m_start.y - 3.f && m_cursor_y < m_end.y + 3.f;

			if (m_value && data->m_left_click && !data->m_ignore && m_is_inside)
			{
				*m_value = !*m_value;
				data->m_ignore = true;
			}

			draw(data);
		}
	};

	class c_float_slider : public c_control
	{
		float* m_value;
		float m_min;
		float m_max;
		float m_step;

		const char* m_text;
		const char* m_suffix;
		const char* m_formatting;

		vec2 m_slider_start;
		float m_slider_pos;

		float m_slider_width;
		float m_slider_height;

		vec2 m_minus_pos;
		vec2 m_minus_bounds;

		vec2 m_plus_pos;
		vec2 m_plus_bounds;

	public:
		c_float_slider()
		{
			m_slider_width = 150.f;
			m_slider_height = 6.f;

			m_bounds = vec2(m_slider_width, m_slider_height);

			m_minus_bounds = vec2(10.f, 10.f);
			m_plus_bounds = vec2(10.f, 10.f);
		}

	public:
		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 21.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 20.f, m_start.y - 3.f }, m_text, text_color);
			}

			text_color = { 255, 255, 255, data->m_alpha };

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_slider_start.x - 1.f, m_slider_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));

			// inner background
			color_t background_color_top(69, 69, 69, data->m_alpha);
			color_t background_color_bottom(44, 44, 44, data->m_alpha);

			ctx.m_renderer->draw_gradient_rect({ m_slider_start.x, m_slider_start.y, m_bounds.x, m_bounds.y }, background_color_top, background_color_top, background_color_bottom, background_color_bottom);

			// slider bar
			color_t slider_color_top = data->m_color; //should be menu colour setting
			color_t slider_color_bottom
			(
				slider_color_top.r() > 25 ? slider_color_top.r() - 25 : 0,
				slider_color_top.g() > 25 ? slider_color_top.g() - 25 : 0,
				slider_color_top.b() > 25 ? slider_color_top.b() - 25 : 0
			);

			ctx.m_renderer->draw_gradient_rect({ m_slider_start.x, m_slider_start.y, m_slider_pos, m_bounds.y }, slider_color_top, slider_color_top, slider_color_bottom, slider_color_bottom);

			// plus/minus
			if (m_min != *m_value)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_minus_pos.x + (m_minus_bounds.x * 0.5f) + 1.f, m_minus_pos.y + 2.f - (m_minus_bounds.y * 0.5f) + 1.f }, xors("-"), text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_minus_pos.x + (m_minus_bounds.x * 0.5f), m_minus_pos.y + 2.f - (m_minus_bounds.y * 0.5f) }, xors("-"), text_color);
			}

			if (m_max != *m_value)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_plus_pos.x + (m_plus_bounds.x * 0.5f) + 1.f, m_plus_pos.y + 2.f - (m_plus_bounds.y * 0.5f) + 1.f }, xors("+"), text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_plus_pos.x + (m_plus_bounds.x * 0.5f), m_plus_pos.y + 2.f - (m_plus_bounds.y * 0.5f) }, xors("+"), text_color);
			}

			std::string text_value(std::string(m_formatting) + std::string(m_suffix));

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_slider_start.x + m_slider_pos + 1.f, m_slider_start.y + 1.f }, std::to_string(*m_value), text_shadow);
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_slider_start.x + m_slider_pos, m_slider_start.y }, std::to_string(*m_value), text_color);
		}

		void handle(c_window_data* data, const char* name, float* setting, float min, float max, float step = 1.f, std::string formatting = "%.2f", std::string suffix = "")
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = strlen(name) > 1 ? name : nullptr;
			m_min = min;
			m_max = max;
			m_step = step;

			m_value = setting;

			// ? like character from some spic language			// xor bug
			if (suffix[0] == 194)
			{
				// get rid of that pesky lil guy
				m_suffix = suffix.erase(0, 1).c_str();
			}
			else
			{
				// normal suffix nothing to see here
				m_suffix = suffix.c_str() ? suffix.c_str() : "";
			}

			// ? like character from some spic language			// xor bug
			if (formatting[0] == 194)
			{
				// get rid of that pesky lil guy
				m_formatting = formatting.erase(0, 1).c_str();
			}
			else
			{
				// normal suffix nothing to see here
				m_formatting = formatting.c_str() ? formatting.c_str() : "";
			}

			m_start = vec2(data->m_x, data->m_y);
			m_slider_start = vec2(m_start.x + 20.f, m_start.y);
			m_end = m_slider_start + m_bounds;

			m_minus_pos = m_slider_start;
			m_minus_pos.x -= m_minus_bounds.x + 10.f;

			m_plus_pos = m_slider_start;
			m_plus_pos.x += m_slider_width + 10.f;

			data->m_y += 15.f;

			if (m_text)
			{
				m_slider_start.y += 15.f;
				m_minus_pos.y += 15.f;
				m_plus_pos.y += 15.f;

				m_end.y += 15.f;
				data->m_y += 15.f;
			}

			m_is_inside = m_cursor_x > m_slider_start.x - 1.0f && m_cursor_x < m_end.x + 1.0f && m_cursor_y > m_slider_start.y - 1.0f && m_cursor_y < m_end.y + 1.0f;

			if (m_dragging && !data->m_left_click)
				m_dragging = false;

			if (m_value)
			{
				*m_value = math::clamp<float>(*m_value, m_min, m_max);

				if (m_dragging)
				{
					float dragged_pos = math::clamp<float>(m_cursor_x - m_slider_start.x, 0, m_slider_width);
					*m_value = m_min + ((m_max - m_min) * dragged_pos / m_slider_width);
				}

				if (data->m_first_click && !data->m_ignore)
				{
					if (m_is_inside)
					{
						m_dragging = true;
						data->m_ignore = true;
					}
					else if (m_cursor_y > m_minus_pos.y && m_cursor_y < (m_minus_pos.y + m_minus_bounds.y))
					{
						if (m_cursor_x > m_minus_pos.x && m_cursor_x < (m_minus_pos.x + m_minus_bounds.x))
						{
							*m_value = math::clamp<float>(*m_value - m_step, m_min, m_max);
							data->m_ignore = true;
						}
						else if (m_cursor_x > m_plus_pos.x && m_cursor_x < (m_plus_pos.x + m_plus_bounds.x))
						{
							*m_value = math::clamp<float>(*m_value + m_step, m_min, m_max);
							data->m_ignore = true;
						}
					}
				}

				m_slider_pos = ((*m_value - m_min) / (m_max - m_min) * m_slider_width);
				m_slider_pos = math::clamp<float>(m_slider_pos, 0.f, m_slider_width * data->m_animation_progress);
			}

			draw(data);
		}
	};

	class c_slider : public c_control
	{
		int* m_value;
		int m_min;
		int m_max;
		int m_step;

		const char* m_text;
		const char* m_suffix;

		vec2 m_slider_start;
		float m_slider_pos;

		float m_slider_width;
		float m_slider_height;

		vec2 m_minus_pos;
		vec2 m_minus_bounds;

		vec2 m_plus_pos;
		vec2 m_plus_bounds;

	public:
		c_slider()
		{
			m_slider_width = 150.f;
			m_slider_height = 6.f;

			m_bounds = vec2(m_slider_width, m_slider_height);

			m_minus_bounds = vec2(10.f, 10.f);
			m_plus_bounds = vec2(10.f, 10.f);
		}

	public:
		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 21.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 20.f, m_start.y - 3.f }, m_text, text_color);
			}

			text_color = { 255, 255, 255, data->m_alpha };

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_slider_start.x - 1.f, m_slider_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));

			// inner background
			color_t background_color_top(69, 69, 69, data->m_alpha);
			color_t background_color_bottom(44, 44, 44, data->m_alpha);

			ctx.m_renderer->draw_gradient_rect({ m_slider_start.x, m_slider_start.y, m_bounds.x, m_bounds.y }, background_color_top, background_color_top, background_color_bottom, background_color_bottom);

			// slider bar
			color_t slider_color_top = data->m_color; //should be menu colour setting
			color_t slider_color_bottom
			(
				slider_color_top.r() > 25 ? slider_color_top.r() - 25 : 0,
				slider_color_top.g() > 25 ? slider_color_top.g() - 25 : 0,
				slider_color_top.b() > 25 ? slider_color_top.b() - 25 : 0
			);

			ctx.m_renderer->draw_gradient_rect({ m_slider_start.x, m_slider_start.y, m_slider_pos, m_bounds.y }, slider_color_top, slider_color_top, slider_color_bottom, slider_color_bottom);

			// plus/minus
			if (m_min != *m_value)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_minus_pos.x + (m_minus_bounds.x * 0.5f) + 1.f, m_minus_pos.y + 2.f - (m_minus_bounds.y * 0.5f) + 1.f }, xors("-"), text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_minus_pos.x + (m_minus_bounds.x * 0.5f), m_minus_pos.y + 2.f - (m_minus_bounds.y * 0.5f) }, xors("-"), text_color);
			}

			if (m_max != *m_value)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_plus_pos.x + (m_plus_bounds.x * 0.5f) + 1.f, m_plus_pos.y + 2.f - (m_plus_bounds.y * 0.5f) + 1.f }, xors("+"), text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_plus_pos.x + (m_plus_bounds.x * 0.5f), m_plus_pos.y + 2.f - (m_plus_bounds.y * 0.5f) }, xors("+"), text_color);
			}

			std::string text_value(std::string("%d") + std::string(m_suffix));
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_slider_start.x + m_slider_pos + 1.f, m_slider_start.y + 1.f }, std::to_string(*m_value), text_shadow);
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_slider_start.x + m_slider_pos, m_slider_start.y }, std::to_string(*m_value), text_color);
		}

		void handle(c_window_data* data, const char* name, int* setting, int min, int max, int step = 1, std::string suffix = "")
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = strlen(name) > 1 ? name : nullptr;
			m_min = min;
			m_max = max;
			m_step = step;

			m_value = setting;

			// ? like character from some spic language			// xor bug
			if (suffix[0] == 194)
			{
				// get rid of that pesky lil guy
				m_suffix = suffix.erase(0, 1).c_str();
			}
			else
			{
				// normal suffix nothing to see here
				m_suffix = suffix.c_str() ? suffix.c_str() : "";
			}

			m_start = vec2(data->m_x, data->m_y);
			m_slider_start = vec2(m_start.x + 20.f, m_start.y);
			m_end = m_slider_start + m_bounds;

			m_minus_pos = m_slider_start;
			m_minus_pos.x -= m_minus_bounds.x + 10.f;

			m_plus_pos = m_slider_start;
			m_plus_pos.x += m_slider_width + 10.f;

			data->m_y += 15.f;

			if (m_text)
			{
				m_slider_start.y += 15.f;
				m_minus_pos.y += 15.f;
				m_plus_pos.y += 15.f;

				m_end.y += 15.f;
				data->m_y += 15.f;
			}

			m_is_inside = m_cursor_x > m_slider_start.x - 1.0f && m_cursor_x < m_end.x + 1.0f && m_cursor_y > m_slider_start.y - 1.0f && m_cursor_y < m_end.y + 1.0f;

			if (m_dragging && !data->m_left_click)
				m_dragging = false;

			if (m_value)
			{
				*m_value = math::clamp<int>(*m_value, m_min, m_max);

				if (m_dragging)
				{
					float dragged_pos = math::clamp<float>(m_cursor_x - m_slider_start.x, 0, m_slider_width);
					*m_value = m_min + ((m_max - m_min) * dragged_pos / m_slider_width);
				}

				if (data->m_first_click && !data->m_ignore)
				{
					if (m_is_inside)
					{
						m_dragging = true;
						data->m_ignore = true;
					}
					else if (m_cursor_y > m_minus_pos.y && m_cursor_y < (m_minus_pos.y + m_minus_bounds.y))
					{
						if (m_cursor_x > m_minus_pos.x && m_cursor_x < (m_minus_pos.x + m_minus_bounds.x))
						{
							*m_value = math::clamp<int>(*m_value - m_step, m_min, m_max);
							data->m_ignore = true;
						}
						else if (m_cursor_x > m_plus_pos.x && m_cursor_x < (m_plus_pos.x + m_plus_bounds.x))
						{
							*m_value = math::clamp<int>(*m_value + m_step, m_min, m_max);
							data->m_ignore = true;
						}
					}
				}

				m_slider_pos = (float(*m_value - m_min) / float(m_max - m_min) * m_slider_width);
				m_slider_pos = math::clamp<float>(m_slider_pos, 0.f, m_slider_width * data->m_animation_progress);
			}

			draw(data);
		}
	};

	class c_button : public c_control
	{
		const char* m_text{};
	public:
		void draw(c_window_data* data)
		{
			color_t color = color_t(36, 36, 36, data->m_alpha);

			if (m_is_inside)
				color = data->m_left_click ? color_t(72, 72, 72, data->m_alpha) : color_t(50, 50, 50, data->m_alpha);

			ctx.m_renderer->draw_filled_rect({ m_start.x - 1.f, m_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_start.x, m_start.y, m_bounds.x, m_bounds.y }, color);

			auto text_color = m_is_inside && data->m_left_click ? data->m_color : color_t(153, 153, 153, data->m_alpha);

			if (m_text)
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + m_bounds.x * 0.5f, m_start.y + (m_bounds.y * 0.5f) - 6.0f }, m_text, text_color);
		}

		void handle(c_window_data* data, const char* text, const std::function<void()>& function)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = text;
			m_start = vec2(data->m_x + 20.f, data->m_y);
			m_bounds = vec2(150.f, 20.f);
			m_end = m_start + m_bounds;

			data->m_y += 25.f;

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_end.x && m_cursor_y > m_start.y && m_cursor_y < m_end.y;

			if (data->m_left_click && !data->m_ignore && m_is_inside)
			{
				function();
				data->m_ignore = true;
			}

			draw(data);
		}
	};

	class c_key_bind : public c_control
	{
		int m_key_type = kt_hold;
		bool m_type_dropdown = false;
		int m_draw_tick{};

		const char* m_text{};
		keysetting_t* m_key{};
		std::vector<std::string> m_items = { xors("hold"), xors("toggle"), xors("always on"), "" };
		const float m_item_height = 16;

		vec2 m_box_start, m_box_end;
		bool m_is_waiting{};

		bool m_is_inside_items{};
		float m_dropdown_end{};

		keytype_t m_type{};
	public:
		void handle(c_window_data* data, const char* text, keysetting_t* key, keytype_t type = kt_all)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = strlen(text) > 1 ? text : nullptr;
			m_key = key;
			m_draw_tick = data->m_draw_counter;
			m_type = type;

			m_start = vec2(data->m_x + 20.f, data->m_y);
			m_box_start = vec2(m_start.x, m_start.y);

			data->m_y += 25.f;

			if (m_text)
			{
				m_box_start.y += 15.f;
				data->m_y += 15.f;
			}

			m_bounds = vec2(150, 20);

			m_end = m_box_start + m_bounds;

			m_box_end = m_end;

			if (m_type_dropdown)
			{
				const float h = 3 * 16;
				m_box_end.y += h + 3.f;
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;
			m_is_inside_items = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if (data->m_left_click && m_is_inside && !data->m_ignore)
			{
				m_is_waiting = true;
				data->m_ignore = true;
			}

			if (m_is_waiting && m_key->type == kt_always)
			{
				data->m_ignore = false;
				m_is_waiting = false;
			}

			static bool clicked = false;
			if (data->m_active)
			{
				if (m_is_inside && data->m_right_click && !clicked)
				{
					m_type_dropdown = !m_type_dropdown;
					clicked = true;
				}

				if (!m_is_inside && m_type_dropdown && m_is_inside_items && data->m_left_click)
				{
					float offset = m_cursor_y - m_end.y;
					m_key->type = (int)(offset / m_item_height);
					clicked = false;

					m_type_dropdown = false;
				}
				else if (!m_is_inside && !m_is_inside_items && data->m_left_click)
				{
					m_type_dropdown = false;
					clicked = false;
				}

				if (!data->m_right_click && clicked)
					clicked = false;

				if (m_type != kt_all)
				{
					m_type_dropdown = false;
					m_key->type = m_type;
				}
			}

			if (m_type_dropdown)
			{
				data->m_ignore = true;
				m_is_waiting = false;
			}

			if (m_is_waiting)
				ctx.m_block_keyinput = true;

			if (m_is_waiting)
			{
				VirtualKeys_t start_key = data->m_ignore ? KEYS_MOUSE1 : KEYS_NONE;

				if (g_input.is_any_key_pressed() == KEYS_ESCAPE)
				{
					m_is_waiting = false;
					m_key->key = KEYS_NONE;
				}
				else if (g_input.is_any_key_pressed() > start_key)
				{
					m_key->key = (g_input.is_any_key_pressed() == KEYS_MOUSE1 || g_input.is_any_key_pressed() == KEYS_MOUSE2) ? KEYS_NONE : g_input.is_any_key_pressed();
					m_is_waiting = false;
					data->m_ignore = true;
				}
			}

			/*static int old_key = KEYS_NONE;
			if ( m_key->type == kt_always )
			{
				if ( m_key->key != KEYS_NONE )
					old_key = m_key->key;
				m_key->key = KEYS_NONE;
			}
			else if ( old_key > KEYS_NONE )
			{
				if ( m_key->key == KEYS_NONE )
					m_key->key = old_key;
				old_key = KEYS_NONE;
			}*/

			draw(data);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 1.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x, m_start.y - 3.f }, m_text, text_color);
			}

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(36, 36, 36, data->m_alpha));

			//supwemacy does m_box_start.y + 4.f, but it looks off (cuz it is off)
			if (m_is_waiting && !m_key->key)
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.0f, m_box_start.y + 3.f }, xors("press key"), data->m_color);

			if (m_key->key > KEYS_NONE || m_key->type == kt_always)
			{
				if (m_key->key > KEYS_NONE)
				{
					const auto yeeter = g_input.get_key_name((VirtualKeys_t)m_key->key);

					int alpha = m_is_waiting ? data->m_color.a() : data->m_alpha;

					if (m_key->type == kt_always)
						alpha = 100;

					if (!m_is_waiting)
						ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.0f, m_box_start.y + 3.f }, yeeter, color_t(153, 153, 153, alpha));
					else
						ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.0f, m_box_start.y + 3.f }, yeeter, color_t(data->m_color.r(), data->m_color.g(), data->m_color.b(), alpha));
				}

				text_color = !m_type_dropdown ? color_t(153, 153, 153, data->m_alpha) : data->m_color;

				if (m_key->key <= KEYS_NONE && m_key->type == kt_always)
					ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.0f, m_box_start.y + 3.f }, m_items[m_key->type], text_color);
				else
				{
					auto buffer_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), m_items[m_key->type]).x;
					ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_bounds.x - (buffer_width + 10.0f), m_box_start.y + 3.f }, m_items[m_key->type], text_color);
				}
			}
		}

		void dropdown(c_window_data* data)
		{
			if (m_draw_tick != data->m_draw_counter)
				return;

			if (m_type_dropdown)
			{
				m_dropdown_end += 2.f;

				if (m_dropdown_end + m_end.y > m_box_end.y)
					m_dropdown_end = m_box_end.y - m_end.y;
			}
			else
			{
				m_dropdown_end -= 2.f;

				if (m_dropdown_end <= 0.f)
					m_dropdown_end = 0.f;
			}

			// shop is closed!
			if (m_dropdown_end <= 0.f)
				return;

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_end.y + 2.f, (m_box_end.x + 1.f) - m_box_start.x, m_bounds.y + 2.f + m_dropdown_end }, color_t(0, 0, 0, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_end.y + 2.f + 1.f, (m_box_end.x) - m_box_start.x, m_bounds.y + 2.f + m_dropdown_end - 1.f }, color_t(36, 36, 36, data->m_alpha));

			for (size_t i = 0; i < m_items.size(); i++)
			{
				float x = m_box_start.x + 10.f;
				float y = m_end.y + 3.f + i * 16;

				if (y + (m_item_height * 0.5f) > m_end.y + m_dropdown_end)
					continue;

				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { x, y }, m_items[i], m_key->type == i ? data->m_color : color_t(153, 153, 153, data->m_alpha));
			}
		}
	};

	struct dropdown_item_t2
	{
		std::string item;
		color_t color = color_t(153, 153, 153);
	};

	class c_dropdown : public c_control
	{
		const char* m_text{};
		std::vector<std::string> m_items{};

		int* m_setting{};
		bool m_is_open{};
		int m_draw_tick{};

		vec2 m_box_start;
		vec2 m_box_end;
		bool m_is_inside_items{};
		float m_dropdown_end{};

		int m_item_offset{};
		int m_max_items{};

		const float m_item_height = 16.f;
	public:

		// god adding an out var would have made skinchanger 10x easier and smaller, ill rewrite later...
		void handle(c_window_data* data, const char* text, const std::vector<std::string>& items, int* setting, int max_items = 0, const bool& disabled = false, std::string * out = nullptr)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			data->m_active = disabled == false;

			m_text = strlen(text) > 1 ? text : nullptr;
			m_items = items;
			m_setting = setting;
			m_draw_tick = data->m_draw_counter;
			m_max_items = max_items < items.size() ? max_items : 0;

			if (disabled)
				m_is_open = false;

			// handle scroll and set m_item_offset
			if (m_is_open && m_max_items)
			{
				int scroll_input = g_input.get_scroll_state();

				/*if ( g_input.is_key_pressed( KEYS_DOWN ) )
					scroll_input = -1;
				else if ( g_input.is_key_pressed( KEYS_UP ) )
					scroll_input = 1;*/

				m_item_offset = math::clamp<int>(m_item_offset - scroll_input, 0, items.size() - m_max_items);
			}
			else
			{
				m_item_offset = 0;
			}

			m_start = vec2(data->m_x + 20.f, data->m_y);
			m_box_start = vec2(m_start.x, m_start.y);

			data->m_y += 25.f;

			if (m_text)
			{
				m_box_start.y += 15.f;
				data->m_y += 15.f;
			}

			m_bounds = vec2(150, 20);

			m_end = m_box_start + m_bounds;

			m_box_end = m_end;

			if (m_is_open)
			{
				float h = m_max_items ? m_max_items * m_item_height : items.size() * m_item_height;
				m_box_end.y += h + 3.f;
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;
			m_is_inside_items = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if (data->m_active && data->m_left_click && !data->m_ignore)
			{
				if (m_is_inside)
				{
					m_is_open = !m_is_open;
					data->m_ignore = true;
				}
				else if (m_is_open && m_is_inside_items)
				{
					float offset = m_cursor_y - m_end.y;
					*m_setting = m_item_offset + (int)(offset / m_item_height);
					data->m_ignore = true;

					m_is_open = false;
				}
				else
				{
					m_is_open = false;
				}
			}

			//clamp setting to avoid cwashing
			*m_setting = math::clamp<int>(*m_setting, 0, math::max<int>(0, items.size() - 1));

			if (out)
				*out = m_items[*m_setting];

			draw(data);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 1.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x, m_start.y - 3.f }, m_text, text_color);
			}

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));

			// inner background
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(36, 36, 36, data->m_alpha));

			text_color = data->m_active ? color_t(153, 153, 153, data->m_alpha) : color_t(92, 92, 92, data->m_alpha);
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.f, m_box_start.y + 3.f }, m_items.at(*m_setting), text_color);

			// bester arrow
			const color_t arrow_color(153, 153, 153, data->m_alpha);

			// draw a lock if not active
			// idk how emojis work
			/*if ( !data->m_active )
				UI::string( color_t( 206, 206, 206 ), m_end.x + 5.0f, m_box_start.y + 9.0f, false, "??" );
				*/

			float x = m_end.x - 11.f;
			float y = m_box_start.y + 9.f;

			if (m_is_open)
			{
				ctx.m_renderer->draw_line({ x + 2.f, y }, { x + 3.f, y }, arrow_color);
				ctx.m_renderer->draw_line({ x + 1.f, y + 1.f }, { x + 4.f, y + 1.f }, arrow_color);
				ctx.m_renderer->draw_line({ x, y + 2.f }, { x + 5.f, y + 2.f }, arrow_color);
			}
			else
			{
				ctx.m_renderer->draw_line({ x + 2.f, y + 2.f }, { x + 3.f, y + 2.f }, arrow_color);
				ctx.m_renderer->draw_line({ x + 1.f, y + 1.f }, { x + 4.f, y + 1.f }, arrow_color);
				ctx.m_renderer->draw_line({ x, y }, { x + 5.f, y }, arrow_color);
			}
		}

		void dropdown(c_window_data* data)
		{
			if (m_draw_tick != data->m_draw_counter)
				return;

			// adjust end of dropdown

			// sorry for ugly code
			// as soon as m_is_open is false, m_box_end is set to bounding of button, so cant be used to just clamp it in 1 line, it would break close animation
			// maybe this is reason supremacy also doesnt have close animation :>
			if (m_is_open)
			{
				m_dropdown_end += 2.f;

				if (m_dropdown_end + m_end.y > m_box_end.y)
					m_dropdown_end = m_box_end.y - m_end.y;
			}
			else
			{
				m_dropdown_end -= 2.f;

				if (m_dropdown_end <= 0.f)
					m_dropdown_end = 0.f;
			}

			// shop is closed!
			if (m_dropdown_end <= 0.f)
				return;

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_end.y + 2.f, (m_box_end.x + 1.f) - m_box_start.x, m_bounds.y + 2.f + m_dropdown_end }, color_t(0, 0, 0, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_end.y + 2.f + 1.f, (m_box_end.x) - m_box_start.x, m_bounds.y + 2.f + m_dropdown_end - 1.f }, color_t(36, 36, 36, data->m_alpha));

			size_t start = m_max_items ? m_item_offset : 0;
			size_t count = m_max_items ? m_max_items : m_items.size();

			for (size_t i = 0; i < count; i++)
			{
				float x = m_box_start.x + 10.f;
				float y = m_end.y + 3.f + i * m_item_height;

				if (y + (m_item_height * 0.5f) > m_end.y + m_dropdown_end)
					continue;

				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { x, y }, m_items.at(start + i), *m_setting == start + i ? data->m_color : color_t(153, 153, 153, data->m_alpha));
			}

			// draw scrollbar
			if (m_is_open && m_max_items && m_max_items < m_items.size())
			{
				float fraction = float(m_max_items) / float(m_items.size());

				float length = m_dropdown_end - 2.f;
				float scroll_size = fraction * length;
				float scroll_step = length / float(m_items.size());

				float scroll_start = scroll_step * m_item_offset;

				ctx.m_renderer->draw_filled_rect({ m_box_end.x - 4.f, m_bounds.y + 4.f + scroll_start, m_box_end.x - 1.f, m_bounds.y + 2.f + scroll_start + scroll_size }, color_t(0, 0, 0, data->m_alpha));
				ctx.m_renderer->draw_line({ m_box_end.x - 3.f, m_end.y + 4.f + scroll_start + 1.f }, { m_box_end.x - 3.f, m_end.y + 2.f + scroll_start + scroll_size - 1.f }, data->m_color);
			}
		}
	};

	struct dropdown_item_t
	{
		bool* m_setting;
		std::string m_text;
	};

	class c_multi_dropdown : public c_control
	{
		const char* m_text{};
		std::vector<dropdown_item_t> m_items{};

		bool m_is_open{};
		int m_draw_tick{};

		vec2 m_box_start;
		vec2 m_box_end;
		bool m_is_inside_items{};
		float m_dropdown_end{};

		const float m_item_height = 16.f;
	public:

		void handle(c_window_data* data, const char* text, const std::vector<dropdown_item_t>& items)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = strlen(text) > 1 ? text : nullptr;
			m_items = items;
			m_draw_tick = data->m_draw_counter;

			m_start = vec2(data->m_x + 20.f, data->m_y);
			m_box_start = vec2(m_start.x, m_start.y);

			data->m_y += 25.f;

			if (m_text)
			{
				m_box_start.y += 15.f;
				data->m_y += 15.f;
			}

			m_bounds = vec2(150, 20);

			m_end = m_box_start + m_bounds;

			m_box_end = m_end;

			if (m_is_open)
			{
				float h = items.size() * m_item_height;
				m_box_end.y += h + 3.f;
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;
			m_is_inside_items = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if (data->m_left_click && !data->m_ignore)
			{
				if (m_is_inside)
				{
					m_is_open = !m_is_open;
					data->m_ignore = true;
				}
				else if (m_is_open && m_is_inside_items)
				{
					float offset = m_cursor_y - m_end.y;
					int clicked_item = math::clamp<int>((int)(offset / m_item_height), 0, math::max<int>(0, m_items.size() - 1));
					*m_items.at(clicked_item).m_setting = !*m_items.at(clicked_item).m_setting;
					data->m_ignore = true;
				}
				else
				{
					m_is_open = false;
				}
			}

			draw(data);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 1.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x, m_start.y - 3.f }, m_text, text_color);
			}

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(36, 36, 36, data->m_alpha));

			std::string items_text;
			if (!m_items.empty())
			{
				for (size_t i = 0; i < m_items.size(); i++)
				{
					if (!*m_items.at(i).m_setting)
						continue;

					const float width_limit = 120.f;

					float new_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), items_text + m_items.at(i).m_text);

					if (new_width > width_limit)
					{
						items_text += xors(", ...");
						break;
					}

					if (!items_text.empty())
						items_text += ", ";

					items_text += m_items.at(i).m_text;
				}
			}
			else
			{
				items_text = "-";
			}

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.f, m_box_start.y + 3.f }, items_text, color_t(153, 153, 153, data->m_alpha));

			// bester arrow
			const color_t arrow_color(153, 153, 153, data->m_alpha);

			float x = m_end.x - 11.f;
			float y = m_box_start.y + 9.f;

			if (m_is_open)
			{
				ctx.m_renderer->draw_line({ x + 2.f, y }, { x + 3.f, y }, arrow_color);
				ctx.m_renderer->draw_line({ x + 1.f, y + 1.f }, { x + 4.f, y + 1.f }, arrow_color);
				ctx.m_renderer->draw_line({ x, y + 2.f }, { x + 5.f, y + 2.f }, arrow_color);
			}
			else
			{
				ctx.m_renderer->draw_line({ x + 2.f, y + 2.f }, { x + 3.f, y + 2.f }, arrow_color);
				ctx.m_renderer->draw_line({ x + 1.f, y + 1.f }, { x + 4.f, y + 1.f }, arrow_color);
				ctx.m_renderer->draw_line({ x, y }, { x + 5.f, y }, arrow_color);
			}
		}

		void dropdown(c_window_data* data)
		{
			if (m_draw_tick != data->m_draw_counter)
				return;

			// adjust end of dropdown

			// sorry for ugly code
			// as soon as m_is_open is false, m_box_end is set to bounding of button, so cant be used to just clamp it in 1 line, it would break close animation
			// maybe this is reason supremacy also doesnt have close animation :>
			if (m_is_open)
			{
				m_dropdown_end += 2.f;

				if (m_dropdown_end + m_end.y > m_box_end.y)
					m_dropdown_end = m_box_end.y - m_end.y;
			}
			else
			{
				m_dropdown_end -= 2.f;

				if (m_dropdown_end <= 0.f)
					m_dropdown_end = 0.f;
			}

			// shop is closed!
			if (m_dropdown_end <= 0.f)
				return;

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_end.y + 2.f, (m_box_end.x + 1.f) - m_box_start.x, m_bounds.y + 2.f + m_dropdown_end }, color_t(0, 0, 0));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_end.y + 2.f + 1.f,(m_box_end.x) - m_box_start.x, m_bounds.y + 2.f + m_dropdown_end - 1.f }, color_t(36, 36, 36));

			for (size_t i = 0; i < m_items.size(); i++)
			{
				float x = m_box_start.x + 10.f;
				float y = m_end.y + 3.f + i * m_item_height;

				if (y + (m_item_height * 0.5f) > m_end.y + 2.f + m_dropdown_end)
					continue;

				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { x, y }, m_items.at(i).m_text, *m_items.at(i).m_setting ? data->m_color : color_t(153, 153, 153));
			}
		}
	};

	class c_color_picker : public c_control
	{
		uint8_t temp_r{};
		uint8_t temp_g{};
		uint8_t temp_b{};

		const char* m_text{};

		bool m_is_open{};
		int m_draw_tick{};

		vec2 m_box_start{};
		vec2 m_box_end{};
		vec2 m_box_bounds{ 20, 8 };

		vec2 m_picker_start{};
		vec2 m_picker_end{};
		vec2 m_picker_bounds{ 256, 256 };

		int m_alpha_value{};
		c_slider m_alpha_slider{};
	public:
		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 21.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 20.f, m_start.y - 3.f }, m_text, text_color);
			}

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_box_bounds.x + 1.f, m_box_bounds.y + 1.f }, color_t(44, 44, 44, data->m_alpha));

			// inner box
			color_t box_color_top(temp_r, temp_g, temp_b, data->m_alpha);
			color_t box_color_bottom
			(
				box_color_top.r() > 25 ? box_color_top.r() - 25 : 0,
				box_color_top.g() > 25 ? box_color_top.g() - 25 : 0,
				box_color_top.b() > 25 ? box_color_top.b() - 25 : 0,
				data->m_alpha
			);

			ctx.m_renderer->draw_gradient_rect({ m_box_start.x, m_box_start.y, m_box_bounds.x, m_box_bounds.y }, box_color_top, box_color_top, box_color_bottom, box_color_bottom);
		}

		void handle(c_window_data* data, const char* text, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a = nullptr)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = text;
			m_start = vec2(data->m_x, data->m_y);
			m_draw_tick = data->m_draw_counter;

			m_box_start = vec2(data->m_x + 150.f, data->m_y);
			m_box_end = m_box_start + m_box_bounds;

			m_picker_start = vec2(m_box_start.x, m_box_start.y + 10);
			m_picker_end = m_picker_start + m_picker_bounds;

			temp_r = *r;
			temp_g = *g;
			temp_b = *b;
			// setup coord for next item
			data->m_y += 15.f;

			if (a)
			{
				m_alpha_value = *a;

				m_alpha_slider.handle(data, "", &m_alpha_value, 0, 255);

				m_alpha_value = math::clamp<int>(m_alpha_value, 0, 255);

				*a = m_alpha_value;
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if (m_is_inside && data->m_first_click && !data->m_ignore)
			{
				m_is_open = true;
				data->m_ignore = true;
			}

			if (m_is_open)
			{
				if (m_cursor_x > m_picker_start.x && m_cursor_x < m_picker_end.x && m_cursor_y > m_picker_start.y && m_cursor_y < m_picker_end.y)
				{
					//set temp color
					float height = m_picker_bounds.y - 2;
					float half_height = height * 0.5f;

					float x_offset = m_cursor_x - (m_picker_start.x + 1);
					float y_offset = m_cursor_y - (m_picker_start.y + 1);

					float hue = x_offset / height;

					color_t color;

					if (y_offset > half_height)
					{
						float brightness = 1.f - math::clamp<float>((y_offset - half_height) / half_height, 0.f, 1.f);
						color = color_t::from_hsb(hue, 1.f, brightness);
					}
					else
					{
						float saturation = math::clamp<float>(y_offset / half_height, 0.f, 1.f);
						color = color_t::from_hsb(hue, saturation, 1.f);
					}

					temp_r = color.r();
					temp_g = color.g();
					temp_b = color.b();
				}

				if (!data->m_left_click)
				{
					*r = temp_r;
					*g = temp_g;
					*b = temp_b;
					m_is_open = false;
				}
			}

			draw(data);
		}

		void picker(c_window_data* data)
		{
			if (!m_is_open)
				return;

			if (m_draw_tick != data->m_draw_counter)
				return;

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_picker_start.x, m_picker_start.y, m_bounds.x, m_bounds.y }, color_t(0, 0, 0, data->m_alpha));

			// picker
			float height = m_picker_bounds.y - 2.f;
			float half_height = height / 2.f;

			for (float i = 0.f; i < height; i += 1.f)
			{
				float hue = i / height;

				color_t top = color_t::from_hsb(hue, 0.f, 1.f);
				color_t middle = color_t::from_hsb(hue, 1.f, 1.f);
				color_t bottom = color_t::from_hsb(hue, 1.f, 0.f);

				*top.a_ptr() = data->m_alpha;
				*middle.a_ptr() = data->m_alpha;
				*bottom.a_ptr() = data->m_alpha;

				ctx.m_renderer->draw_gradient_rect({ m_picker_start.x + 1.f + i, m_picker_start.y + 1.f, 1.f, half_height }, top, top, middle, middle);
				ctx.m_renderer->draw_gradient_rect({ m_picker_start.x + 1.f + i, m_picker_start.y + 1.f + half_height, 1.f, half_height }, middle, middle, bottom, bottom);
			}
		}
	};

	class c_text_input : public c_control
	{
		const char* m_text{};
		char* m_setting{};

		vec2 m_box_start;

		float m_draw_time{};
	public:
		bool m_is_waiting{};

		void handle(c_window_data* data, const char* text, char* setting, int buffer_size = 32, const bool& numbers_only = false)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = strlen(text) > 1 ? text : nullptr;
			m_setting = setting;

			m_start = vec2(data->m_x + 20.f, data->m_y);
			m_box_start = vec2(m_start.x, m_start.y);

			data->m_y += 25.f;

			if (m_text)
			{
				m_box_start.y += 15.f;
				data->m_y += 15.f;
			}

			m_bounds = vec2(150, 20);

			m_end = m_box_start + m_bounds;

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;

			if (data->m_first_click)
			{
				if (!data->m_ignore && m_is_inside)
				{
					m_is_waiting = true;
					data->m_ignore = true;
				}
				else
				{
					m_is_waiting = false;
				}
			}

			if (data->m_right_click && m_is_inside)
				memset(m_setting, 0, sizeof(m_setting));

			if (m_is_waiting)
				ctx.m_block_keyinput = true;

			if (!numbers_only || isdigit(data->m_keyboard_input))
			{
				if (m_is_waiting && data->m_keyboard_input > 0)
				{
					size_t length = strlen(m_setting);

					if (data->m_keyboard_input == 0x8) //backspace
					{
						if (length > 0)
							m_setting[length - 1] = 0;
					}
					else if (length < buffer_size /*&& UI::text_width( m_setting ) < ( m_bounds.x - 20.f ) */)
					{
						if (util::is_valid_text(data->m_keyboard_input))
						{
							m_setting[length] = data->m_keyboard_input;
							m_setting[length + 1] = 0;
						}
					}
				}
			}

			draw(data);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			if (m_text)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 1.f, m_start.y - 2.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x, m_start.y - 3.f }, m_text, text_color);
			}

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_bounds.x + 1.f, m_bounds.y + 1.f }, color_t(0, 0, 0, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(36, 36, 36, data->m_alpha));

			if (strlen(m_setting) > 0)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_end.x - 12.0f, m_box_start.y + 3.0f }, "x", color_t(150, 150, 150, data->m_alpha));

				const bool inside_x = m_cursor_x > m_end.x - 14.0f && m_cursor_x < m_end.x - 4.0f && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;

				if (inside_x && data->m_left_click)
					memset(m_setting, 0, sizeof(m_setting));
			}

			std::string draw_text = m_setting;
			auto setting_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), m_setting).x;
			auto w_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), "W").x;

			const float max_time = 5.f;

			if (m_is_waiting)
			{
				if (setting_width > (m_bounds.x - 20.f))
				{
					int overlapping_chars = ((setting_width - (m_bounds.x)) / w_width) + 2;
					draw_text.erase(0, overlapping_chars);
				}

				if ((fmodf(m_draw_time, 1.f)) > 0.5f)
					draw_text += '|';
			}
			else
			{
				// 1 step closer to skeet polish!?
				if (strlen(m_setting) < 1)
					ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.f, m_box_start.y + 3.f }, xors("..."), color_t(100, 100, 100, data->m_alpha));
			}

			if ((setting_width > (m_bounds.x - 20.f)) && !m_is_waiting)
			{
				float temp = 1.5f;

				//amount we "overshoot" the box
				int overlapping_chars = ((setting_width - (m_bounds.x)) / w_width) + 1;

				//printf("%d\n", overshot_ammount);

				if (m_draw_time < temp)
					draw_text.resize(draw_text.length() - overlapping_chars);
				else if (m_draw_time < max_time - temp)
					draw_text.erase(0, overlapping_chars);
				else
				{
					draw_text.erase(draw_text.length() - overlapping_chars / 2, overlapping_chars / 2);
					draw_text.erase(0, overlapping_chars / 2);
				}
			}

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.f, m_box_start.y + 3.f }, draw_text, color_t(153, 153, 153, data->m_alpha));

			//m_draw_time += csgo.m_globals()->frametime;

			if (m_draw_time > max_time)
				m_draw_time = 0.f;
		}
	};
}