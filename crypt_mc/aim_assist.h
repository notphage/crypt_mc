#pragma once

class c_aim_assist : public c_feature
{
	std::vector<float> m_vec;
	float m_last_scale = -1.f;

	void scan();
	float get_x_speed() const;
	float get_y_speed() const;
public:
	void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;

	void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
};
