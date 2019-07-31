#pragma once

class c_throw : public c_feature
{
	std::vector<int> find_item(const std::shared_ptr<c_player>& self, int min, int max, jclass clazz) const;

	int find_healing(const std::shared_ptr<c_player>& self) const;
	int find_debuff(const std::shared_ptr<c_player>& self) const;
	int find_pearl(const std::shared_ptr<c_player>& self) const;
public:
	void on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override;

	void on_render(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
	void on_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>&, const std::shared_ptr<c_world>&) override { }
};
