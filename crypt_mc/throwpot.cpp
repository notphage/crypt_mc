#include "context.h"
#include "throw.h"

void c_throwpot::on_tick(const std::shared_ptr<c_game>& mc, const std::shared_ptr<c_player>& self, const std::shared_ptr<c_world>& world)
{
	auto stage_switch = [&](int slot, bool& is_running)
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

				printf("stage 0: old_slot %i current_slot %i\n", old_slot, self->get_current_slot());
				stage++;
				is_running = true;
				break;
			}

			case 1:
			{
				if (clock() - delay > ctx.m_settings.player_throw_delay && self->get_held_item())
				{
					self->send_use_item(self->get_held_item());

					printf("stage 1: old_slot %i current_slot %i\n", old_slot, self->get_current_slot());
					stage++;
				}
				break;
			}

			case 2:
			{
				self->set_current_slot(old_slot);

				delay = clock();
				old_slot = -1;

				printf("stage 2: old_slot %i current_slot %i\n", old_slot, self->get_current_slot());
				stage = 0;
				is_running = false;

				//Set disabled
				break;
			}
		}
	};

	if (!ctx.m_settings.player_throw)
		return;

	static bool healing_running = false;
	if ((!valid_keystate(ctx.m_settings.player_throwhealing_key) && !healing_running))
		return;
	else if (true)
		if (auto slot = find_healing(self); slot >= 0)
			stage_switch(slot, healing_running);

	static bool pearl_running = false;
	if ((!valid_keystate(ctx.m_settings.player_throwpearl_key) && !pearl_running) || healing_running)
		return;
	else
	{
		auto cls_pearl = ctx.get_class_loader()->find_class(xors("net.minecraft.item.ItemEnderPearl"));

		auto slot = find_item(self, 36, 45, cls_pearl);

		if (slot.empty())
			return;

		stage_switch(slot.at(0), pearl_running);
	}

	static bool debuff_running = false;
	if ((!valid_keystate(ctx.m_settings.player_throwdebuff_key) && !debuff_running) || healing_running || pearl_running)
		return;
	else if (auto slot = find_debuff(self); slot >= 0)
		stage_switch(slot, debuff_running);
}

std::vector<int> c_throwpot::find_item(const std::shared_ptr<c_player>& self, int min, int max, jclass clazz) const
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

int c_throwpot::find_healing(const std::shared_ptr<c_player>& self) const
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

int c_throwpot::find_debuff(const std::shared_ptr<c_player>& self) const
{
	return -1;
}

int c_throwpot::find_pearl(const std::shared_ptr<c_player>& self) const
{
	auto class_loader = ctx.get_class_loader();
	auto cls_pearl = class_loader->find_class(xors("net.minecraft.item.ItemEnderPearl"));

	auto items = find_item(self, 36, 45, cls_pearl);

	for (auto item : items)
		return item;

	return -1;
}
