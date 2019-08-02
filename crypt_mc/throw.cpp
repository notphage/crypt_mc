#include "context.h"
#include "throw.h"

#include <array>

void c_throw::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	auto stage_switch = [&](int slot, bool& is_running, keysetting_t& key)
	{
		static int stage = 0;
		static int old_slot = -1;
		static long delay = clock();

		switch (stage)
		{
			case 0:
			{
				old_slot = self->get_current_slot();

				self->set_current_slot(slot);

				delay = clock();

				stage++;
				is_running = true;
				break;
			}

			case 1:
			{
				if (clock() - delay > 150 + ctx.m_settings.player_throw_delay + util::random(-30, 30) && self->get_held_item())
				{
					self->send_use_item(self->get_held_item());

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
				self->set_current_slot(old_slot);

				old_slot = -1;

				stage = 0;
				is_running = false;
				g_input.toggle_key(key);

				//Set disabled
				break;
			}
		}
	};

	if (!ctx.m_settings.player_throw)
		return;

	static bool healing_running = false;
	if ((valid_keystate(ctx.m_settings.player_throwhealing_key) || healing_running))
		if (self->get_max_health() - self->get_health() >= 8.f || healing_running)
		{
			if (auto slot = find_healing(self); slot >= 0)
				stage_switch(slot, healing_running, ctx.m_settings.player_throwhealing_key);
			else
				g_input.toggle_key(ctx.m_settings.player_throwhealing_key);
		}
		else
			g_input.toggle_key(ctx.m_settings.player_throwhealing_key);

	static bool pearl_running = false;
	if ((valid_keystate(ctx.m_settings.player_throwpearl_key) || pearl_running) && !healing_running)
	{
		if (auto slot = find_pearl(self); slot >= 0)
			stage_switch(slot, pearl_running, ctx.m_settings.player_throwpearl_key);
		else
			g_input.toggle_key(ctx.m_settings.player_throwpearl_key);
	}
	else
		g_input.toggle_key(ctx.m_settings.player_throwpearl_key);

	static bool debuff_running = false;
	if ((valid_keystate(ctx.m_settings.player_throwdebuff_key) || debuff_running) && !healing_running && !pearl_running)
	{
		if (auto slot = find_debuff(self); slot >= 0)
			stage_switch(slot, debuff_running, ctx.m_settings.player_throwpearl_key);
		else
			g_input.toggle_key(ctx.m_settings.player_throwdebuff_key);
	}
	else
		g_input.toggle_key(ctx.m_settings.player_throwdebuff_key);
}

std::vector<int> c_throw::find_item(const std::shared_ptr<c_player>& self, int min, int max, jclass clazz) const
{
	std::vector<int> items;

	for (auto i = min; i < max; ++i)
	{
		if (self->get_stack(i))
		{
			auto item_stack = self->get_stack(i);
			auto item = self->get_item(item_stack);

			if (ctx.m_jni->IsInstanceOf(item, clazz))
				items.push_back(i);
		}
	}

	return items;
}

int c_throw::find_healing(const std::shared_ptr<c_player>& self) const
{
	auto class_loader = ctx.get_class_loader();
	auto cls_potion = class_loader->find_class(xors("net.minecraft.item.ItemPotion"));
	auto cls_soup = class_loader->find_class(xors("net.minecraft.item.ItemSoup"));
	auto cls_potion_effect = class_loader->find_class(xors("net.minecraft.potion.PotionEffect"));
	auto cls_list = class_loader->find_class(xors("java.util.List"));

	for (auto i = 36; i < 45; ++i)
	{
		if (self->get_stack(i))
		{
			auto item_stack = self->get_stack(i);
			auto item = self->get_item(item_stack);

			if (ctx.m_jni->IsInstanceOf(item, cls_potion) && ctx.m_settings.player_throwhealing_potions)
			{
				auto potion = item;

				auto obj_effects = self->get_effects(potion, item_stack);

				if (obj_effects)
				{
					auto mid_to_array = ctx.m_jni->GetMethodID(cls_list, xors("toArray"), xors("()[Ljava/lang/Object;"));
					auto arr_effects = static_cast<jobjectArray>(ctx.m_jni->CallObjectMethod(obj_effects, mid_to_array));

					for (jint x = 0; x < ctx.m_jni->GetArrayLength(arr_effects); x++)
					{
						auto mid_get = ctx.m_jni->GetMethodID(cls_list, xors("get"), xors("(I)Ljava/lang/Object;"));

						auto effect = ctx.m_jni->CallObjectMethod(obj_effects, mid_get, x);

						if (self->get_potion_id(effect) == 6)
							return i - 36;
					}
				}
			}
			else if (ctx.m_jni->IsInstanceOf(item, cls_soup) && ctx.m_settings.player_throwhealing_soup)
				return i - 36;
		}
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
		debuffs.emplace_back(15, &ctx.m_settings.player_throwdebuff_blind);
		debuffs.emplace_back(18, &ctx.m_settings.player_throwdebuff_weakness);
		debuffs.emplace_back(20, &ctx.m_settings.player_throwdebuff_wither);
	)


	auto class_loader = ctx.get_class_loader();
	auto cls_potion = class_loader->find_class(xors("net.minecraft.item.ItemPotion"));
	auto cls_potion_effect = class_loader->find_class(xors("net.minecraft.potion.PotionEffect"));
	auto cls_list = class_loader->find_class(xors("java.util.List"));

	for (auto i = 36; i < 45; ++i)
	{
		if (self->get_stack(i))
		{
			auto item_stack = self->get_stack(i);
			auto item = self->get_item(item_stack);

			if (ctx.m_jni->IsInstanceOf(item, cls_potion) && ctx.m_settings.player_throwhealing_potions)
			{
				auto potion = item;

				auto obj_effects = self->get_effects(potion, item_stack);

				if (obj_effects)
				{
					auto mid_to_array = ctx.m_jni->GetMethodID(cls_list, xors("toArray"), xors("()[Ljava/lang/Object;"));
					auto arr_effects = static_cast<jobjectArray>(ctx.m_jni->CallObjectMethod(obj_effects, mid_to_array));

					for (jint x = 0; x < ctx.m_jni->GetArrayLength(arr_effects); x++)
					{
						auto mid_get = ctx.m_jni->GetMethodID(cls_list, xors("get"), xors("(I)Ljava/lang/Object;"));

						auto effect = ctx.m_jni->CallObjectMethod(obj_effects, mid_get, x);

						for (auto id : debuffs)
							if (self->get_potion_id(effect) == id.first && *id.second)
								return i - 36;
					}
				}
			}
		}
	}

	return -1;
}

int c_throw::find_pearl(const std::shared_ptr<c_player>& self) const
{
	auto class_loader = ctx.get_class_loader();
	auto cls_pearl = class_loader->find_class(xors("net.minecraft.item.ItemEnderPearl"));

	auto items = find_item(self, 36, 45, cls_pearl);

	for (auto item : items)
		return item - 36;

	return -1;
}
