#pragma once

class c_speed : public c_feature
{
	bool m_on_ground = false;

    void bhop(std::shared_ptr<c_player> self) const;
    void slowhop(std::shared_ptr<c_player> self) const;
    void custom(std::shared_ptr<c_player> self) const;
    void glidehop(std::shared_ptr<c_player> self) const;
    void minihop(std::shared_ptr<c_player> self) const;
    void yport(std::shared_ptr<c_player> self) const;
    void vanilla(std::shared_ptr<c_player> self) const;

	void longjump(std::shared_ptr<c_player> self) const;

    void jump(std::shared_ptr<c_player> self, double height) const;

	void set_speed(std::shared_ptr<c_player> self, float speed) const;

	static float get_direction(std::shared_ptr<c_player> self);
    static double get_base_speed(std::shared_ptr<c_player> self);
public:
	void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;

	void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
};
