#pragma once

class c_flight : public c_feature
{
    void set_speed(const std::shared_ptr<c_player>& self, float speed) const;
    static float get_direction(const std::shared_ptr<c_player>& self);
public:
	c_flight(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_flight::on_get_time, this, _1, _2, _3), feature_type::FEATURE_GET_TIME);
	}

	void on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;
};
