#pragma once

class c_base_speed : public c_feature
{
protected:
	bool m_on_ground = false;

	static void jump(const std::shared_ptr<c_player>& self, float height);
	void set_speed(const std::shared_ptr<c_player>& self, float speed) const;
	static float get_direction(const std::shared_ptr<c_player>& self);
	static float get_base_speed(const std::shared_ptr<c_player>& self);

public:
	c_base_speed(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{}

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};

class c_speed : public c_base_speed
{
    void bhop(const std::shared_ptr<c_player>& self) const;
    void slowhop(const std::shared_ptr<c_player>& self) const;
    void custom(const std::shared_ptr<c_player>& self) const;
    void glidehop(const std::shared_ptr<c_player>& self) const;
    void minihop(const std::shared_ptr<c_player>& self) const;
    void yport(const std::shared_ptr<c_player>& self) const;
    void vanilla(const std::shared_ptr<c_player>& self) const;

public:
	c_speed(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_speed(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_speed::on_get_time, this, _1, _2, _3), feature_type::FEATURE_GET_TIME);
	}

	void on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};

class c_long_jump : public c_base_speed
{
public:
	c_long_jump(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_speed(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_long_jump::on_get_time, this, _1, _2, _3), feature_type::FEATURE_GET_TIME);
	}

	void on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};

class c_air_control : public c_base_speed
{
public:
	c_air_control(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_speed(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_air_control::on_get_time, this, _1, _2, _3), feature_type::FEATURE_GET_TIME);
	}

	void on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};
