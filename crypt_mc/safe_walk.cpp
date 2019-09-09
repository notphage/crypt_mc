#include "context.h"
#include "safe_walk.h"

void c_safe_walk::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	if (!ctx.m_settings.movement_safe_walk || !valid_keystate(ctx.m_settings.movement_safe_walk_key))
		return;
	
	if (!world->get_block(self->origin_x(), self->origin_y() - 1.0f, self->origin_z())) 
	{
		printf("%s", "return 1");
		return;
	}
	
	//printf("%i", world->get_block(self->origin_x(), self->origin_y() - 1.0f, self->origin_z()));

	//mc->set_key_bind_state(mc->get_sneak_key_code(), world->get_block(self->origin_x(), self->origin_y() - 1.0f, self->origin_z())->get_id() == 0);
}