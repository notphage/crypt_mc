#pragma once

class c_visuals : public c_feature
{
	void draw_box_2d(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const vec3&);
	void draw_box_3d(const std::shared_ptr<c_player>&, const vec3&);
	void draw_snap_lines(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const vec3&);
	void draw_name_tags(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const vec3&);
public:
	void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;

	void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
};
