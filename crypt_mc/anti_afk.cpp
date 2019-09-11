#include "context.h"
#include "anti_afk.h"

void c_anti_afk::on_get_time(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	// proper frametime
	static auto old = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
	
	static auto delay = util::random(60000, 180000);
	
	if (self->get_forward() != 0.f || self->get_strafing() != 0.f)
	{
		old = now;
		delay = util::random(60000, 180000);
	}

	if (delta > delay && self->is_collided_vertically() && self->is_on_ground())
	{
		old = now;
		delay = util::random(60000, 180000);

		self->set_motion_y(0.42f);
	}
}
