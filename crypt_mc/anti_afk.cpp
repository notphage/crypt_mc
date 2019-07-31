#include "context.h"
#include "anti_afk.h"

void c_anti_afk::on_tick(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	// proper frametime
	static auto old = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	float delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - old).count();
	
	static auto delay = util::random_delay(60000, 180000);
	
	if (ctx.m_settings.player_anti_afk && delta > delay)
	{
		old = now;
		delay = util::random_delay(60000, 180000);

		self->set_motion_y(0.41999998688697815);
	}
}
