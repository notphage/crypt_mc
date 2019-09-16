#include "context.h"
#include "throw.h"

#include <array>

void c_throw::on_update(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	auto stage_switch = [&](int slot, bool& is_running, keysetting_t& key)
	{
		static int stage = 0;
		static int old_slot = -1;
		static long delay = GetTickCount64();

		switch (stage)
		{
			case 0:
			{
				old_slot = self->get_current_slot();

				self->set_current_slot(slot);

				delay = GetTickCount64();

				stage++;
				is_running = true;
				break;
			}

			case 1:
			{
				if (GetTickCount64() - delay > 75 + ctx.m_settings.player_throw_delay + util::random(-30, 30) && self->get_held_item())
				{
					self->send_use_item(self->get_held_item());

					delay = GetTickCount64();
					stage++;
				}
				else if (!self->get_held_item())
				{
					self->set_current_slot(old_slot);
					old_slot = -1;

					stage = 0;
					is_running = false;
				}
				break;
			}

			case 2:
			{
				if (GetTickCount64() - delay > 75 + util::random(-30, 30))
				{
					self->set_current_slot(old_slot);

					old_slot = -1;

					stage = 0;
					is_running = false;
					g_input.toggle_key(key);

					//Set disabled
				}
				break;
			}
		}
	};

	static bool healing_running = false;
	if ((valid_keystate(ctx.m_settings.player_throwhealing_key) || healing_running))
	{
		if (self->get_max_health() - self->get_health() >= 7.f || healing_running)
		{
			if (const auto slot = find_healing(self); slot >= 0)
				stage_switch(slot, healing_running, ctx.m_settings.player_throwhealing_key);
			else
				g_input.toggle_key(ctx.m_settings.player_throwhealing_key);
		}
		else
			g_input.toggle_key(ctx.m_settings.player_throwhealing_key);
	}

	static bool pearl_running = false;
	if ((valid_keystate(ctx.m_settings.player_throwpearl_key) || pearl_running) && !healing_running)
	{
		if (const auto slot = find_pearl(self); slot >= 0)
			stage_switch(slot, pearl_running, ctx.m_settings.player_throwpearl_key);
		else
			g_input.toggle_key(ctx.m_settings.player_throwpearl_key);
	}
	else
		g_input.toggle_key(ctx.m_settings.player_throwpearl_key);

	static bool debuff_running = false;
	if ((valid_keystate(ctx.m_settings.player_throwdebuff_key) || debuff_running) && !healing_running && !pearl_running)
	{
		if (const auto slot = find_debuff(self); slot >= 0)
			stage_switch(slot, debuff_running, ctx.m_settings.player_throwpearl_key);
		else
			g_input.toggle_key(ctx.m_settings.player_throwdebuff_key);
	}
	else
		g_input.toggle_key(ctx.m_settings.player_throwdebuff_key);
}

int c_throw::find_healing(const std::shared_ptr<c_player>& self) const
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

int c_throw::find_debuff(const std::shared_ptr<c_player>& self) const
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
}

int c_throw::find_pearl(const std::shared_ptr<c_player>& self) const
{
	auto items = self->find_item(36, 45, PEARL);

	for (auto item : items)
		return item - 36;

	return -1;
}
