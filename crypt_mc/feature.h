#pragma once

class c_feature
{
protected:
	bool enabled = false;

public:
	virtual void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) = 0;
	virtual void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) = 0;
	virtual void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) = 0;
	virtual void on_atan2(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) = 0;
};
