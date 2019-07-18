#pragma once

namespace UI
{
	class c_control
	{
	public:
		vec2 m_start;
		vec2 m_bounds;
		vec2 m_end;

		int m_cursor_x;
		int m_cursor_y;

		float m_alpha = 255;

		bool m_is_inside;

		size_t m_current_stage;

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
		int m_alpha = 255;
		float m_animation_progress = 0.f;
		bool m_active = true;
	};

	class c_window : public c_control
	{
		bool m_is_open{};
		bool m_was_open{};
		bool m_should_open = true;

		vec2 m_window_size{};
		c_window_data m_data{};
		const float m_edge_size = 8.f;
		bool m_is_inside_edge{};

		void draw_box()
		{
			// draw black background
			ctx.m_renderer->draw_filled_rect({ m_start.x, m_start.y, m_bounds.x, m_bounds.y }, color_t(20, 20, 20, 255));

			// draw grey outline box 1px in
			ctx.m_renderer->draw_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f }, color_t(159, 159, 159, 200));
		}

		void draw_topbar()
		{
			ctx.m_renderer->draw_gradient_rect({ m_start.x + 2.f, m_start.y + 2.f, 68.f, 4.f }, color_t(187, 10, 30), color_t(234, 10, 30), color_t(187, 10, 30), color_t(234, 10, 30));
			ctx.m_renderer->draw_gradient_rect({ m_start.x + 70.f, m_start.y + 2.f, 128.f, 4.f }, color_t(234, 10, 30), color_t(187, 10, 30), color_t(234, 10, 30), color_t(187, 10, 30));
			ctx.m_renderer->draw_gradient_rect({ m_start.x + 198.f, m_start.y + 2.f, 120.f, 4.f }, color_t(187, 10, 30), color_t(234, 10, 30), color_t(187, 10, 30), color_t(234, 10, 30));

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_title), { 98, 10 }, "C", color_t(234, 10, 30));
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_title), { 125, 10 }, "RYPT", color_t(255, 255, 255));
		}

	public:
		c_window()
		{
			m_start = vec2(0, 0);
			m_window_size = vec2(320, 240);
			m_bounds = vec2(320, 240);
			m_end = m_start + m_bounds;
		}

		void handle()
		{
			ctx.m_block_keyinput = false;

			m_data.m_draw_counter++;
			m_data.m_keyboard_input = g_input.get_keyboard_input();

			*m_data.m_color.a_ptr() = m_data.m_alpha;

			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + m_bounds.x && m_cursor_y > m_start.y && m_cursor_y < m_start.y + m_bounds.y;
			m_is_inside_edge = m_is_inside && ((m_cursor_x > m_start.x + m_bounds.x - m_edge_size || m_cursor_x < m_start.x + m_edge_size) || (m_cursor_y > m_start.y + m_bounds.y - m_edge_size || m_cursor_y < m_start.y + m_edge_size));

			// update data
			m_data.m_first_click = !m_data.m_left_click && g_input.is_key_pressed(KEYS_MOUSE1);
			m_data.m_left_click = g_input.is_key_pressed(KEYS_MOUSE1);
			m_data.m_right_click = g_input.is_key_pressed(KEYS_MOUSE2);
			m_data.m_animation_progress = std::clamp<float>(m_data.m_animation_progress + (ctx.m_frametime * 15.f), 0.f, 1.f);

			if (!m_data.m_left_click)
				m_data.m_ignore = false;

			draw_box();
			draw_topbar();
		}

		void column_center()
		{
			m_data.m_x = m_start.x + 66.f;
			m_data.m_y = m_start.y + 90.f;
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

	class c_button : public c_control
	{
		const char* m_text{};
	public:
		void draw(c_window_data* data)
		{
			color_t color = color_t(36, 36, 36, data->m_alpha);

			if (m_is_inside)
				color = data->m_left_click ? color_t(72, 72, 72, data->m_alpha) : color_t(50, 50, 50, data->m_alpha);

			ctx.m_renderer->draw_rect({ m_start.x, m_start.y, m_bounds.x, m_bounds.y }, color_t(10, 10, 10, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f , m_bounds.y - 1.f }, color);

			if (m_text)
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + m_bounds.x * 0.41f, m_start.y + (m_bounds.y * 0.5f) - 8.f }, m_text, m_is_inside && data->m_left_click ? data->m_color : color_t(153, 153, 153, data->m_alpha));
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


	class c_text_input : public c_control
	{
		const char* m_text{};
		char* m_setting{};

		bool m_password = false;

		vec2 m_box_start;

		float m_draw_time{};
	public:
		bool m_is_waiting{};

		void handle(c_window_data* data, const char* text, char* setting, bool password = false, int buffer_size = 32, const bool& numbers_only = false)
		{
			g_input.get_cursor_pos(m_cursor_x, m_cursor_y);

			m_text = strlen(text) > 1 ? text : nullptr;
			m_setting = setting;
			m_password = password;

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
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x + 1.f, m_start.y - 1.f }, m_text, text_shadow);
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_start.x, m_start.y - 2.f }, m_text, text_color);
			}

			ctx.m_renderer->draw_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(10, 10, 10, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x + 1.f, m_box_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 1.f }, color_t(36, 36, 36, data->m_alpha));

			if (strlen(m_setting) > 0)
			{
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_end.x - 12.0f, m_box_start.y + 3.0f }, "x", color_t(150, 150, 150, data->m_alpha));

				const bool inside_x = m_cursor_x > m_end.x - 14.0f && m_cursor_x < m_end.x - 4.0f && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;

				if (inside_x && data->m_left_click)
					memset(m_setting, 0, sizeof(m_setting));
			}

			std::string draw_text = m_setting;
			auto text_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), m_setting).x;
			auto w_width = ctx.m_renderer->get_text_extent(ctx.m_renderer->get_font(font_normal), "W").x;

			const float max_time = 5.f;

			if (m_is_waiting)
			{
				if (text_width > (m_bounds.x - 20.f))
				{
					int overlapping_chars = (int)((text_width - m_bounds.x) / w_width) + 2;
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

			if ((text_width > (m_bounds.x - 20.f)) && !m_is_waiting)
			{
				float temp = 1.5f;

				//amount we "overshoot" the box
				int overlapping_chars = (int)((text_width - (m_bounds.x)) / w_width) + 1;

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

			if (m_password)
				for (auto&& c : draw_text)
					c = '*';

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.f, m_box_start.y + 3.f }, draw_text.c_str(), color_t(153, 153, 153, data->m_alpha));

			//m_draw_time += csgo.m_globals()->frametime;

			if (m_draw_time > max_time)
				m_draw_time = 0.f;
		}
	};
}