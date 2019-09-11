#pragma once

struct keysetting_t;

enum feature_type
{
	FEATURE_GET_TIME,
	FEATURE_UPDATE,
	FEATURE_ATAN2,
	FEATURE_SWAP_BUFFERS
};

using feature_func_t = void(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

class c_feature
{
protected:
	bool m_enabled = false;
	bool m_ran_callback = true;
	bool* m_setting = nullptr;
	keysetting_t* m_keybind = nullptr;

	std::vector<std::pair<std::function<feature_func_t>, feature_type>> m_callbacks;

	void register_feature(std::function<feature_func_t> callback, feature_type type);

public:
	c_feature(bool* setting, keysetting_t* keybind = nullptr)
		: m_setting(setting), m_keybind(keybind)
	{}

	void run(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&, feature_type type);

	virtual void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) = 0;
	virtual void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) = 0;
};
