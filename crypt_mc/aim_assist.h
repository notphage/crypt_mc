#pragma once

struct target_t
{
	bool m_valid = false;
	std::shared_ptr<c_player> m_target;
	float m_distance;
	float m_pitch_change;
	float m_yaw_change;
};

class c_aim_assist : public c_feature
{

	std::vector<float> m_vec;
	float m_last_scale = -1.f;

	void find_best_point(const std::shared_ptr<c_player>& self, const std::shared_ptr<c_player>& player, const vec3& trace_origin, target_t& target_data);
	void find_target(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world, target_t& best_target_data);

	void scan();
	float get_x_speed();
	float get_y_speed();
public:
	c_aim_assist(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_aim_assist::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};
