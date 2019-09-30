#pragma once

class c_fullbright : public c_feature
{
public:
	c_fullbright(bool* setting, keysetting_t* keybind = nullptr)
		: c_feature(setting, keybind)
	{
		using namespace std::placeholders;

		register_feature(std::bind(&c_fullbright::on_update, this, _1, _2, _3), feature_type::FEATURE_UPDATE);
	}

	void on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&);

	void on_enable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;
	void on_disable(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;
};
