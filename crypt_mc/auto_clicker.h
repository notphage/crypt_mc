#pragma once

class c_auto_clicker : public c_feature
{
	bool m_left_click = true;
	bool m_set_change = false;
	bool m_blocking = false;

	uint32_t m_min_cps = ctx.m_settings.combat_auto_clicker_min_cps;
	uint32_t m_max_cps = ctx.m_settings.combat_auto_clicker_max_cps;
	uint32_t m_clicks_skip = 0;
	uint32_t m_clicks_until_skip = 57;
	uint32_t m_skip_click_amt = 2;
	uint32_t m_skip_click_count = 0;
	uint32_t m_clicks_change = 0;
	uint32_t m_clicks_until_change = 50;
	uint32_t m_clicks_until_escape = 9;
	uint32_t m_current_delay = 0;
	uint32_t m_clicks = 0;
	uint32_t m_last_click = 0;

	float m_multiplier = 0.5f;

public:
	c_auto_clicker(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_auto_clicker::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void reset();
	void set_tick_delay(int tick);
	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};
