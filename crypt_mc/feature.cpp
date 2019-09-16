#include "context.h"
#include "feature.h"

void c_feature::run(const std::shared_ptr<c_game>& game, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world, feature_type type)
{
	if ((!*m_setting) || !(m_keybind != nullptr && g_input.is_key_pressed(*m_keybind) && (m_keybind->key > KEYS_NONE || m_keybind->type == kt_always)))
	{
		if (m_enabled)
		{
			on_disable(game, self, world);
			m_enabled = false;
		}

		return;
	}

	if (!m_enabled)
	{
		on_enable(game, self, world);
		m_enabled = true;
	}

	for (auto&& callback : m_callbacks)
		if (callback.second == type)
			callback.first(game, self, world);
}

void c_feature::register_feature(std::function<feature_func_t> callback, feature_type type)
{
	m_callbacks.emplace_back(callback, type);
}
