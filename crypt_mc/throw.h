#pragma once

class c_base_throw : public c_feature
{
protected:
	uint8_t m_stage = 0;

	int m_old_slot = -1;

	uint64_t m_delay = 0;

	bool m_healing_running = false;
	bool m_debuff_running = false;
	bool m_pearl_running = false;

	int stage_switch(const std::shared_ptr<c_player>& self, int slot);

public:
	c_base_throw(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{ }

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { };
};

class c_throw_healing : public c_base_throw
{
	int find_healing(const std::shared_ptr<c_player>& self) const;
public:
	c_throw_healing(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_throw(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_throw_healing::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};

class c_throw_debuff : public c_base_throw
{
	int find_debuff(const std::shared_ptr<c_player>& self) const;
public:
	c_throw_debuff(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_throw(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_throw_debuff::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};

class c_throw_pearl : public c_base_throw
{
	int find_pearl(const std::shared_ptr<c_player>& self) const;
public:
	c_throw_pearl(bool* setting, keysetting_t* keybind = nullptr)
		: c_base_throw(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_throw_pearl::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);
};
