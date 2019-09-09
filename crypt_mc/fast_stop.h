#pragma once

class c_fast_stop : public c_feature
{
public:
	void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }

	void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;
};
