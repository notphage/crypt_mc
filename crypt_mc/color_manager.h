#pragma once

class c_color_manager
{
	static float m_sinebow_progress;

public:
	enum modifier
	{
		sinebow
	};

	void handle_colors();
	void add_color(modifier mod, bool* condition, const std::array<uint8_t*, 3>& color_rgba);

private:
	std::vector<std::tuple<modifier, bool*, std::array<uint8_t*, 3>>> m_colors;
};