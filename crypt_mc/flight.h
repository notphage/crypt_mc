#pragma once

class c_flight : public c_feature
{
    void set_speed(std::shared_ptr<c_player> self, float speed) const;
    static float get_direction(std::shared_ptr<c_player> self);
public:
    void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;

    void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
};
