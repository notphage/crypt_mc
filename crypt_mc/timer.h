#pragma once

class c_base_timer : public c_feature
{
public:
	c_base_timer(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{ }

	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;
	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};

class c_positive_timer : public c_base_timer
{
public:
	c_positive_timer(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_timer(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_positive_timer::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};

class c_negative_timer : public c_base_timer
{
public:
	c_negative_timer(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_timer(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_negative_timer::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};

