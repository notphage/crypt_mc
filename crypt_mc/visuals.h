#pragma once

class c_visuals : public c_feature
{
	struct esp_box_t
	{
		int32_t minx, maxx, miny, maxy;
		int32_t centerx, centery;

		esp_box_t()
			: minx(std::numeric_limits<int32_t>::max()), maxx(std::numeric_limits<int32_t>::min()), miny(std::numeric_limits<int32_t>::max()), maxy(std::numeric_limits<int32_t>::min())
		{ }

		bool create_player(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_player>& ent);
	};

	struct esp_player_t
	{
		std::shared_ptr<c_player> m_player;
		float m_dist;

		esp_player_t(const std::shared_ptr<c_player>& player, float distance)
			: m_player(player), m_dist(distance)
		{ }

		void draw(JNIEnv* jni, const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world);
		void draw_health(const vec2& bot, const vec2& top, int health, int max_health, uint8_t alpha, const color_t& outline_color);
		void draw_snaplines(const vec2& bot, const color_t& snaplines_color, const color_t& outline_color);
	};

	static void draw_ent_box(const color_t& color, esp_box_t& box, bool outlined, const color_t& outline_color);
	static void draw_ent_corner_box(const color_t& color, esp_box_t& box, bool outlined, const color_t& outline_color);
	static void draw_ent_filled_box(const color_t& color, esp_box_t& box);

public:
	c_visuals(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_visuals::on_swap_buffers, this, _1, _2, _3), feature_type::FEATURE_SWAP_BUFFERS);
	}

	void on_swap_buffers(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};
