#include "context.h"
#include "sprint.h"

void c_sprint::on_time(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
    if (!ctx.m_settings.movement_sprint || !valid_keystate(ctx.m_settings.movement_sprint_key))
        return;

    if (g_input.is_key_pressed(KEYS_W) || g_input.is_key_pressed(KEYS_A) || g_input.is_key_pressed(KEYS_S) || g_input.is_key_pressed(KEYS_D))
    {
        self->set_sprinting(true);
    }
}
