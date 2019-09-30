#include "context.h"
#include "fullbright.h"

float gamma = 0;

void c_fullbright::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (mc->get_gamma() < 200.0f)
		mc->set_gamma(mc->get_gamma() + 0.08f);

	//mc->disable_light_map();
}

void c_fullbright::on_enable(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	gamma = mc->get_gamma();
}

void c_fullbright::on_disable(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	mc->set_gamma(gamma);

	gamma = 0;
}
