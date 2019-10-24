#pragma once

class c_velocity : public c_feature
{


public:
	c_velocity(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_velocity::on_atan2, this, _1, _2, _3), feature_type::FEATURE_ATAN2);
	}

	void on_atan2(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	std::pair<bool, bool> check_player_pos(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};
