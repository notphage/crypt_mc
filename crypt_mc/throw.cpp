#include "context.h"
#include "throw.h"

#include <array>

int c_throw_healing::find_healing(const std::shared_ptr<c_player>& self) const
{
	if (ctx.m_settings.player_throwhealing_potions)
	{
		auto items = self->find_item(36, 45, POTION);

		for (auto&& potion : items)
		{
			auto item_stack = self->get_stack(potion);
			if (item_stack)
			{
				auto item = self->get_item(item_stack);
				if (item)
				{
					auto obj_effects = self->get_effects(item, item_stack);
					if (obj_effects)
					{
						auto effect_id = self->get_effects_id(obj_effects);

						if (effect_id == -1)
							continue;

						if (effect_id == 6)
							return potion - 36;
					}
				}
			}
		}
	}
	else if (ctx.m_settings.player_throwhealing_soup)
	{
		auto items = self->find_item(36, 45, SOUP);

		for (auto&& soup : items)
			return soup - 36;
	}

	return -1;
}

void c_throw_healing::on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	if (!m_debuff_running && !m_pearl_running)
	{
		if (self->get_max_health() - self->get_health() >= 7.f || m_healing_running)
		{
			if (auto slot = find_healing(self); slot != -1 || m_healing_running)
			{
				m_healing_running = true;

				if (stage_switch(self, slot) == 0)
				{
					m_enabled = false;
					m_healing_running = false;
				}
			}
			else
				g_input.toggle_key(*m_keybind);
		}
		else
			g_input.toggle_key(*m_keybind);
	}
}

int c_throw_debuff::find_debuff(const std::shared_ptr<c_player>& self) const
{
	static std::vector<std::pair<int, bool*>> debuffs;

	anti_rin(
		debuffs.emplace_back(7, &ctx.m_settings.player_throwdebuff_damage);
		debuffs.emplace_back(19, &ctx.m_settings.player_throwdebuff_poison);
		debuffs.emplace_back(2, &ctx.m_settings.player_throwdebuff_slow);
		debuffs.emplace_back(18, &ctx.m_settings.player_throwdebuff_weakness);
	)

	auto items = self->find_item(36, 45, POTION);

	for (auto&& potion : items)
	{
		auto item_stack = self->get_stack(potion);
		if (item_stack)
		{
			auto item = self->get_item(item_stack);
			if (item)
			{
				auto obj_effects = self->get_effects(item, item_stack);
				if (obj_effects)
				{
					auto effect_id = self->get_effects_id(obj_effects);

					if (effect_id == -1)
						continue;

					for (auto id : debuffs)
						if (effect_id == id.first && *id.second)
							return potion - 36;
				}
			}
		}
	}

	return -1;
}

void c_throw_debuff::on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	if (!m_healing_running && !m_pearl_running)
	{
		if (auto slot = find_debuff(self); slot != -1 || m_debuff_running)
		{
			m_debuff_running = true;

			if (stage_switch(self, slot) == 0)
			{
				m_enabled = false;
				m_debuff_running = false;
			}
		}
		else
			g_input.toggle_key(*m_keybind);
	}
}

int c_throw_pearl::find_pearl(const std::shared_ptr<c_player>& self) const
{
	auto items = self->find_item(36, 45, PEARL);

	for (auto item : items)
		return item - 36;

	return -1;
}

void c_throw_pearl::on_update(const std::shared_ptr<c_game>&, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>&)
{
	if (!m_healing_running && !m_debuff_running)
	{
		if (auto slot = find_pearl(self); slot != -1 || m_pearl_running)
		{
			m_pearl_running = true;

			if (stage_switch(self, slot) == 0)
			{
				m_enabled = false;
				m_pearl_running = false;
			}
		}
		else
			g_input.toggle_key(*m_keybind);
	}
}

int c_base_throw::stage_switch(const std::shared_ptr<c_player>& self, int slot)
{
	switch (m_stage)
	{
		case 0:
		{
			m_old_slot = self->get_current_slot();
			self->set_current_slot(slot);

			++m_stage;
			m_delay = GetTickCount64();

			return m_stage;
		}

		case 1:
		{

			if (GetTickCount64() - m_delay > ctx.m_settings.player_throw_delay)
			{
				g_input.press_mouse(false);
				++m_stage;
				m_delay = GetTickCount64();
			}

			return m_stage;
		}

		case 2:
		{
			if (GetTickCount64() - m_delay > ctx.m_settings.player_throw_delay)
			{
				g_input.release_mouse(false);
				self->set_current_slot(m_old_slot);

				m_old_slot = -1;
				m_stage = 0;
				g_input.toggle_key(*m_keybind);
			}

			return m_stage;
		}
	}

	return -1;
}
