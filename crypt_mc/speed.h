#pragma once

class c_speed : public c_feature
{
	bool m_on_ground = false;

    void bhop(const std::shared_ptr<c_player>& self) const;
    void slowhop(const std::shared_ptr<c_player>& self) const;
    void custom(const std::shared_ptr<c_player>& self) const;
    void glidehop(const std::shared_ptr<c_player>& self) const;
    void minihop(const std::shared_ptr<c_player>& self) const;
    void yport(const std::shared_ptr<c_player>& self) const;
    void vanilla(const std::shared_ptr<c_player>& self) const;

	void longjump(const std::shared_ptr<c_player>& self) const;

	static void jump(const std::shared_ptr<c_player>& self, float height);

	void set_speed(const std::shared_ptr<c_player>& self, float speed) const;

	static float get_direction(const std::shared_ptr<c_player>& self);
    static float get_base_speed(const std::shared_ptr<c_player>& self);
public:
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;

	void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_atan2(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
};
