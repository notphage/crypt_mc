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
			ctx.m_renderer->draw_rect({ m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 3.f }, color_t(159, 159, 159, 200));
		}

		void draw_topbar()
		{
			ctx.m_renderer->draw_gradient_rect({ m_start.x + 2.f, m_start.y + 2.f, 68.f, 4.f }, color_t(187, 10, 30), color_t(234, 10, 30), color_t(187, 10, 30), color_t(234, 10, 30));
			ctx.m_renderer->draw_gradient_rect({ m_start.x + 70.f, m_start.y + 2.f, 128.f, 4.f }, color_t(234, 10, 30), color_t(187, 10, 30), color_t(234, 10, 30), color_t(187, 10, 30));
			ctx.m_renderer->draw_gradient_rect({ m_start.x + 198.f, m_start.y + 2.f, 120.f, 4.f }, color_t(187, 10, 30), color_t(234, 10, 30), color_t(187, 10, 30), color_t(234, 10, 30));

			ctx.m_renderer->string(ctx.m_renderer->get_font(font_title), { 98, 10 }, xors("C"), color_t(234, 10, 30));
			ctx.m_renderer->string(ctx.m_renderer->get_font(font_title), { 125, 10 }, xors("RYPT"), color_t(255, 255, 255));
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

				m_item_offset = std::clamp<int>(m_item_offset - scroll_input, 0, items.size() - m_max_items);
			}
			else
			{
				m_item_offset = 0;
			}

			m_start = vec2(data->m_x + 20.f, data->m_y - 15.f);
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
			*m_setting = std::clamp<int>(*m_setting, 0, std::max<int>(0, items.size() - 1));

			if (out)
				* out = m_items[*m_setting];

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
			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_bounds.x + 2.f, m_bounds.y + 2.f }, color_t(10, 10, 10, data->m_alpha));

			// inner background
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(36, 36, 36, data->m_alpha));

			if (!m_items.empty())
				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { m_box_start.x + 10.f, m_box_start.y + 3.f }, m_items.at(*m_setting).c_str(), data->m_active ? color_t(153, 153, 153, data->m_alpha) : color_t(92, 92, 92, data->m_alpha));

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
				m_dropdown_end += 3.f;

				if (m_dropdown_end + m_end.y > m_box_end.y)
					m_dropdown_end = m_box_end.y - m_end.y;
			}
			else
			{
				m_dropdown_end -= 3.f;

				if (m_dropdown_end <= 0.f)
					m_dropdown_end = 0.f;
			}

			// shop is closed!
			if (m_dropdown_end <= 0.f)
				return;

			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_end.y + 2.f, (m_box_end.x + 2.f) - m_box_start.x, m_bounds.y + m_dropdown_end - 21.f }, color_t(10, 10, 10, data->m_alpha));
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_end.y + 3.f, (m_box_end.x) - m_box_start.x, m_bounds.y + m_dropdown_end - 23.f }, color_t(36, 36, 36, data->m_alpha));

			size_t start = m_max_items ? m_item_offset : 0;
			size_t count = m_max_items ? m_max_items : m_items.size();

			for (size_t i = 0; i < count; i++)
			{
				float x = m_box_start.x + 10.f;
				float y = m_end.y + 3.f + i * m_item_height;

				if (y + (m_item_height * 0.5f) > m_end.y + m_dropdown_end)
					continue;

				ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { x, y }, m_items.at(start + i).c_str(), *m_setting == start + i ? data->m_color : color_t(153, 153, 153, data->m_alpha));
			}

			// draw scrollbar
			if (m_is_open && m_max_items && m_max_items < m_items.size())
			{
				float fraction = float(m_max_items) / float(m_items.size());

				float length = m_dropdown_end - 2.f;
				float scroll_size = fraction * length;
				float scroll_step = length / float(m_items.size());

				float scroll_start = scroll_step * m_item_offset;

				ctx.m_renderer->draw_line({ m_box_end.x - 3.f, m_end.y + 4.f + scroll_start + 1.f }, { m_box_end.x - 3.f, m_end.y + 2.f + scroll_start + scroll_size - 1.f }, data->m_color);
			}
		}
	};

	struct console_item_t
	{
		console_item_t(const std::string& text, const color_t& text_col, const color_t& background)
			: m_text(text), m_text_col(text_col), m_background(background)
		{ }

		std::string m_text;
		color_t m_text_col;
		color_t m_background;
	};

	class c_console : public c_control
	{
		std::deque<console_item_t> m_items{};

		int m_draw_tick{};

		vec2 m_box_start;
		vec2 m_box_end;

		uint32_t m_max_items = 5;

		static constexpr float m_item_height = 16.f;
	public:

		void handle(c_window_data* data)
		{
			m_draw_tick = data->m_draw_counter;
			m_start = vec2(data->m_x - 40.f, data->m_y - 10.f);
			m_box_start = vec2(m_start.x, m_start.y);

			if (m_items.size() > m_max_items)
			{
				for (auto i = 0; i < m_items.size() - m_max_items; i++)
					m_items.pop_back();
			}

			data->m_y += 78.f;

			m_bounds = vec2(268, 80);

			m_end = m_box_start + m_bounds;

			m_box_end = m_end;

			float h = m_max_items ? m_max_items * m_item_height : m_items.size() * m_item_height;
			m_box_end.y += h + 3.f;

			draw(data);
		}

		void draw(c_window_data* data)
		{
			color_t text_color(206, 206, 206, data->m_alpha);
			color_t text_shadow(0, 0, 0, data->m_alpha);

			// outer edge
			ctx.m_renderer->draw_filled_rect({ m_box_start.x - 1.f, m_box_start.y - 1.f, m_bounds.x + 2.f, m_bounds.y + 2.f }, color_t(10, 10, 10, data->m_alpha));

			// inner background
			ctx.m_renderer->draw_filled_rect({ m_box_start.x, m_box_start.y, m_bounds.x, m_bounds.y }, color_t(36, 36, 36, data->m_alpha));

			if (!m_items.empty())
			{
				for (size_t i = 0; i < m_items.size(); i++)
				{
					float x = m_box_start.x + 10.f;
					float y = m_start.y + i * m_item_height;

					if (y + (m_item_height * 0.5f) > m_end.y)
						continue;

					ctx.m_renderer->draw_filled_rect({ m_box_start.x, y, m_bounds.x, 16.f }, m_items.at(i).m_background);
					ctx.m_renderer->string(ctx.m_renderer->get_font(font_normal), { x, y - 1.f }, m_items.at(i).m_text, m_items.at(i).m_text_col);
				}
			}
		}

		void insert(const std::string& str, const color_t& text_col)
		{
			static bool off_color = false;
			m_items.emplace_front(str, text_col, off_color ? color_t(50, 50, 50) : color_t(36, 36, 36));
			off_color = !off_color;
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
				A_memset(m_setting, 0, sizeof(m_setting));

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
					else if (length < buffer_size)
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
					A_memset(m_setting, 0, sizeof(m_setting));
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