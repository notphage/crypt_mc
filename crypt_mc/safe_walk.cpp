#include "context.h"
#include "safe_walk.h"

void c_safe_walk::on_get_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	//printf("%.2f %.2f %.2f\n", self->origin_x(), self->aabb_min_y(), self->origin_z());

	const auto block = world->get_block(self->origin_x(), self->aabb_min_y() - .5f, self->origin_z());
	if (!block || !self->is_on_ground())
		return;

	mc->set_key_bind_state(mc->get_sneak_key_code(), block->get_id() == 0);
}