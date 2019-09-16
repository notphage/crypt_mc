#pragma once

class c_throw : public c_feature
{
	int find_healing(const std::shared_ptr<c_player>& self) const;
	int find_debuff(const std::shared_ptr<c_player>& self) const;
	int find_pearl(const std::shared_ptr<c_player>& self) const;
public:
	c_throw(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_throw::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};
