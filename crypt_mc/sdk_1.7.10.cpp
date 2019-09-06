#include "context.h"
#include "sdk_1.7.10.h"
#include <iostream>
#include <conio.h>

struct block_fields
{
	jmethodID mid_from_block = nullptr;

	jclass block = nullptr;
};

struct class_loader_fields
{
	jfieldID fid_class_loader = nullptr;

	jmethodID mid_get_class_loader = nullptr;
	jmethodID mid_find_class = nullptr;
	
	jobject obj_class_loader = nullptr;
};

struct world_fields
{
	jfieldID fid_players = nullptr;
	jmethodID mid_get_block = nullptr;
	jmethodID mid_get_ent = nullptr;
	jmethodID mid_to_array = nullptr;
	jmethodID mid_render_world = nullptr;

	jclass world = nullptr;
	jclass entity_renderer = nullptr;
	jclass cls_list = nullptr;

	jobject obj_world = nullptr;
};

struct player_fields
{
	jfieldID fid_max_hurt_time = nullptr;
	jfieldID fid_hurt_time = nullptr;
	jfieldID fid_current_slot = nullptr;
	jfieldID fid_xmotion = nullptr;
	jfieldID fid_ymotion = nullptr;
	jfieldID fid_zmotion = nullptr;
	jfieldID fid_old_xorigin = nullptr;
	jfieldID fid_old_yorigin = nullptr;
	jfieldID fid_old_zorigin = nullptr;
	jfieldID fid_xorigin = nullptr;
	jfieldID fid_yorigin = nullptr;
	jfieldID fid_zorigin = nullptr;
	jfieldID fid_is_dead = nullptr;
	jfieldID fid_collided_vertically = nullptr;
	jfieldID fid_collided_horizontally = nullptr;
	jfieldID fid_on_ground = nullptr;
	jfieldID fid_move_strafing = nullptr;
	jfieldID fid_move_forward = nullptr;
	jfieldID fid_pitch = nullptr;
	jfieldID fid_yaw = nullptr;
	jfieldID fid_prev_yaw = nullptr;
	jfieldID fid_width = nullptr;
	jfieldID fid_height = nullptr;
	jfieldID fid_move_speed = nullptr;
	jfieldID fid_step_height = nullptr;
	jfieldID fid_inventory = nullptr;
	jfieldID fid_inventory_player = nullptr;
	jfieldID fid_obj_capabilities = nullptr;
	jfieldID fid_armor_inventory = nullptr;
	jfieldID fid_aabb = nullptr;
	jfieldID fid_aabb_min_x = nullptr;
	jfieldID fid_aabb_min_y = nullptr;
	jfieldID fid_aabb_min_z = nullptr;
	jfieldID fid_aabb_max_x = nullptr;
	jfieldID fid_aabb_max_y = nullptr;
	jfieldID fid_aabb_max_z = nullptr;
	jfieldID fid_ticks_existed = nullptr;

	jmethodID mid_get_active_potion_effect = nullptr;
	jmethodID mid_get_amplifier = nullptr;
	jmethodID mid_is_potion_active = nullptr;
	jmethodID mid_is_invisible = nullptr;
	jmethodID mid_is_sneaking = nullptr;
	jmethodID mid_is_sprinting = nullptr;
	jmethodID mid_in_water = nullptr;
	jmethodID mid_set_sprinting = nullptr;
	jmethodID mid_swing_item = nullptr;
	jmethodID mid_set_sneaking = nullptr;
	jmethodID mid_set_velocity = nullptr;
	jmethodID mid_get_health = nullptr;
	jmethodID mid_get_max_health = nullptr;
	jmethodID mid_get_name = nullptr;
	jmethodID mid_get_held_item = nullptr;
	jmethodID mid_get_item = nullptr;
	jmethodID mid_get_slot = nullptr;
	jmethodID mid_get_stack = nullptr;
	jmethodID mid_potion_id = nullptr;
	jmethodID mid_get_effects = nullptr;
	jmethodID mid_send_use_item = nullptr;
	jmethodID mid_get_eye_height = nullptr;
	jmethodID mid_is_visible = nullptr;
	jmethodID mid_set_position = nullptr;
	jmethodID mid_set_angles = nullptr;

	jclass entity_class = nullptr;
	jclass entity_player_sp_class = nullptr;
	jclass cls_entity_living_base = nullptr;
	jclass entity_player_class = nullptr;
	jclass item_stack_class = nullptr;
	jclass item_sword_class = nullptr;
	jclass item_axe_class = nullptr;
	jclass item_egg_class = nullptr;
	jclass item_snowball_class = nullptr;
	jclass item_block_class = nullptr;
	jclass cls_container = nullptr;
	jclass cls_slot = nullptr;
	jclass cls_item_potion = nullptr;
	jclass cls_potion = nullptr;
	jclass cls_potion_effect = nullptr;
	jclass cls_player_controller = nullptr;
	jclass cls_inventory = nullptr;
	jclass cls_aabb = nullptr;

	bool holding_weapon = false;
	bool holding_projectile = false;
	bool holding_block = false;
};

struct game_fields
{
	jclass minecraft = nullptr;
	jclass render_manager = nullptr;
	jclass font_renderer = nullptr;
	jclass timer_class = nullptr;
	jclass entity_renderer_class = nullptr;
	jclass settings_class = nullptr;
	jclass cls_inventory = nullptr;
	jclass cls_chat = nullptr;
	jclass cls_moving_object_position = nullptr;
	jclass cls_lwjgl_sys_impl = nullptr;
	jclass cls_lwjgl_display = nullptr;
	jclass cls_lwjgl_mouse = nullptr;

	jfieldID fid_minecraft;
	jfieldID fid_entity_renderer_obj;
	jfieldID fid_timer_obj;
	jfieldID fid_game_settings;
	jfieldID fid_render_manager_obj;
	jfieldID fid_font_renderer_obj;
	jfieldID fid_the_player = nullptr;
	jfieldID fid_the_world = nullptr;
	jfieldID fid_current_screen = nullptr;
	jfieldID fid_in_game_has_focus = nullptr;
	jfieldID fid_right_click_delay = nullptr;
	jfieldID fid_timer_speed = nullptr;
	jfieldID fid_player_controller = nullptr;
	jfieldID fid_render_partial_ticks = nullptr;
	jfieldID fid_render_xpos = nullptr;
	jfieldID fid_render_ypos = nullptr;
	jfieldID fid_render_zpos = nullptr;
	jfieldID fid_player_xview = nullptr;
	jfieldID fid_player_yview = nullptr;
	jfieldID fid_pointed_entity = nullptr;
	jfieldID fid_object_mouse_over = nullptr;
	jfieldID fid_entity_hit = nullptr;
	jfieldID fid_view_bobbing = nullptr;
	jfieldID fid_mouse_sensitivity = nullptr;
	jfieldID fid_type_of_hit = nullptr;

	jmethodID mid_get_minecraft = nullptr;
	jmethodID mid_screen_constructor = nullptr;
	jmethodID mid_get_string_width = nullptr;
	jmethodID mid_set_mouse_grabbed = nullptr;
	jmethodID mid_draw_string_with_shadow = nullptr;
	jmethodID mid_enabled_light_map = nullptr;
	jmethodID mid_disable_light_map = nullptr;
	jmethodID mid_setup_camera_transform = nullptr;
	jmethodID mid_get_net_handler = nullptr;
	jmethodID mid_set_in_focus = nullptr;
	jmethodID mid_set_not_in_focus = nullptr;
	jmethodID mid_get_hwnd = nullptr;
	jmethodID mid_set_cursor_pos = nullptr;
	jmethodID mid_get_width = nullptr;
	jmethodID mid_get_height = nullptr;

	jobject obj_game = nullptr;
	jobject obj_render_manager = nullptr;
	jobject obj_font_renderer = nullptr;
	jobject obj_entity_renderer = nullptr;
	jobject obj_settings = nullptr;
	jobject obj_timer = nullptr;

	bool in_chat = false;
	bool in_inventory = false;
};

static block_fields blockfields;
static class_loader_fields classfields;
static world_fields worldfields;
static player_fields playerfields;
static game_fields gamefields;

void c_block_1710::instantiate(jobject block_instance, JNIEnv* _jni)
{
	jni = _jni;

	static bool init_fields = false;

	if (!init_fields)
	{
		auto class_loader = ctx.get_class_loader(jni);

		if (!class_loader)
			return;

		blockfields.block = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("net.minecraft.block.Block")));
		blockfields.mid_from_block = jni->GetStaticMethodID(blockfields.block, xors("func_149682_b"), xors("(Lnet/minecraft/block/Block;)I"));

		init_fields = true;
	}

	id = jni->CallStaticIntMethod(blockfields.block, blockfields.mid_from_block, block_instance);

	jni->DeleteLocalRef(block_instance);
}

void c_class_loader_1710::instantiate(JNIEnv* _jni)
{
	jni = _jni;

	static bool init_fields = false;
	if (!init_fields && ctx.m_client_flavor != VANILLA)
	{
		auto launch = (jclass)jni->FindClass(xors("net/minecraft/launchwrapper/Launch"));
		auto class_loader = (jclass)jni->FindClass(xors("net/minecraft/launchwrapper/LaunchClassLoader"));

		classfields.fid_class_loader = jni->GetStaticFieldID(launch, xors("classLoader"), xors("Lnet/minecraft/launchwrapper/LaunchClassLoader;"));

		classfields.mid_find_class = jni->GetMethodID(class_loader, xors("findClass"), xors("(Ljava/lang/String;)Ljava/lang/Class;"));

		classfields.obj_class_loader = jni->NewGlobalRef(jni->GetStaticObjectField(class_loader, classfields.fid_class_loader));

		init_fields = true;
	}
}

jclass c_class_loader_1710::find_class(const char* name)
{
	std::string class_name(name);
	if (class_name.find('/') != std::string::npos || ctx.m_client_flavor == VANILLA)
		return jni->FindClass(name);

	jstring java_name = jni->NewStringUTF(name);
	jclass clazz = static_cast<jclass>(jni->CallObjectMethod(classfields.obj_class_loader, classfields.mid_find_class, java_name));

	jni->DeleteLocalRef(java_name);
	return clazz;
}

void c_world_1710::instantiate(jobject world_object, JNIEnv* _jni)
{
	jni = _jni;

	world_instance = world_object;

	static bool init_fields = false;

	if (!init_fields)
	{
		auto class_loader = ctx.get_class_loader(jni);

		if (!class_loader)
			return;

		worldfields.world = class_loader->find_class(xors("ahb"));
		worldfields.entity_renderer = class_loader->find_class(xors("blt"));
		worldfields.cls_list = class_loader->find_class(xors("java/util/List"));

		worldfields.fid_players = jni->GetFieldID(worldfields.world, xors("h"), xors("Ljava/util/List;"));
		worldfields.mid_get_block = jni->GetMethodID(worldfields.world, xors("a"), xors("(III)Laji;"));
		worldfields.mid_get_ent = jni->GetMethodID(worldfields.world, xors("a"), xors("(I)Lsa;"));
		worldfields.mid_to_array = jni->GetMethodID(worldfields.cls_list, xors("toArray"), xors("()[Ljava/lang/Object;"));
		worldfields.mid_render_world = jni->GetMethodID(worldfields.entity_renderer, xors("a"), xors("(FJ)V"));

		worldfields.obj_world = jni->NewGlobalRef(jni->GetObjectField(gamefields.obj_game, gamefields.fid_the_world));

		init_fields = true;
	}
}

std::vector<std::shared_ptr<c_player>> c_world_1710::get_players()
{
	std::vector<std::shared_ptr<c_player>> players;

	const jobject obj_player_ents = jni->GetObjectField(world_instance, worldfields.fid_players);
	const jobjectArray arr_player_ents = static_cast<jobjectArray>(jni->CallObjectMethod(obj_player_ents, worldfields.mid_to_array));

	if (!arr_player_ents)
	{
		jni->DeleteLocalRef(obj_player_ents);
		return players;
	}

	for (jint i = 0; i < jni->GetArrayLength(arr_player_ents); i++)
	{
		auto player_ptr = std::make_shared<c_player_1710>();
		player_ptr->instantiate(jni->NewGlobalRef(jni->GetObjectArrayElement(arr_player_ents, i)), jni);
		players.push_back(player_ptr);
	}

	jni->DeleteLocalRef(arr_player_ents);
	jni->DeleteLocalRef(obj_player_ents);

	return players;
}

std::shared_ptr<c_block> c_world_1710::get_block(jint x, jint y, jint z)
{
	auto block_class = jni->CallObjectMethod(world_instance, worldfields.mid_get_block, x, y, z);

	if (!block_class)
		return nullptr;

	auto block_ptr = std::make_shared<c_block_1710>();
	block_ptr->instantiate(block_class, jni);

	return block_ptr;
}

void c_player_1710::instantiate(jobject player_object, JNIEnv* _jni)
{
	jni = _jni;

	player_instance = player_object;

	static bool init_fields = false;

	if (!init_fields)
	{
		auto class_loader = ctx.get_class_loader(jni);

		if (!class_loader)
			return;

		playerfields.entity_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("sa")));
		playerfields.entity_player_sp_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("blk")));
		playerfields.cls_entity_living_base = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("sv")));
		playerfields.entity_player_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("yz")));
		playerfields.item_stack_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("add")));
		playerfields.item_sword_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("aeh")));
		playerfields.item_axe_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("abf")));
		playerfields.cls_container = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("zs")));
		playerfields.cls_slot = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("aay")));
		playerfields.cls_item_potion = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("adp")));
		playerfields.cls_potion = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("rv")));
		playerfields.cls_potion_effect = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("rw")));
		playerfields.cls_player_controller = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bje")));
		playerfields.cls_inventory = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("yx")));
		playerfields.cls_aabb = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("azt")));
		playerfields.item_egg_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("ack")));
		playerfields.item_snowball_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("aed")));
		playerfields.item_block_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("abh")));

		playerfields.fid_max_hurt_time = jni->GetFieldID(playerfields.cls_entity_living_base, xors("ay"), xors("I"));
		playerfields.fid_hurt_time = jni->GetFieldID(playerfields.cls_entity_living_base, xors("ax"), xors("I"));
		playerfields.fid_current_slot = jni->GetFieldID(playerfields.cls_inventory, xors("c"), xors("I"));
		playerfields.fid_xmotion = jni->GetFieldID(playerfields.entity_class, xors("v"), xors("D"));
		playerfields.fid_ymotion = jni->GetFieldID(playerfields.entity_class, xors("w"), xors("D"));
		playerfields.fid_zmotion = jni->GetFieldID(playerfields.entity_class, xors("x"), xors("D"));
		playerfields.fid_old_xorigin = jni->GetFieldID(playerfields.entity_class, xors("S"), xors("D"));
		playerfields.fid_old_yorigin = jni->GetFieldID(playerfields.entity_class, xors("T"), xors("D"));
		playerfields.fid_old_zorigin = jni->GetFieldID(playerfields.entity_class, xors("U"), xors("D"));
		playerfields.fid_xorigin = jni->GetFieldID(playerfields.entity_class, xors("S"), xors("D"));
		playerfields.fid_yorigin = jni->GetFieldID(playerfields.entity_class, xors("T"), xors("D"));
		playerfields.fid_zorigin = jni->GetFieldID(playerfields.entity_class, xors("U"), xors("D"));
		playerfields.fid_is_dead = jni->GetFieldID(playerfields.entity_class, xors("K"), xors("Z"));
		playerfields.fid_collided_vertically = jni->GetFieldID(playerfields.entity_class, xors("F"), xors("Z"));
		playerfields.fid_collided_horizontally = jni->GetFieldID(playerfields.entity_class, xors("E"), xors("Z"));
		playerfields.fid_on_ground = jni->GetFieldID(playerfields.entity_class, xors("D"), xors("Z"));
		playerfields.fid_move_strafing = jni->GetFieldID(playerfields.cls_entity_living_base, xors("bd"), xors("F"));
		playerfields.fid_move_forward = jni->GetFieldID(playerfields.cls_entity_living_base, xors("be"), xors("F"));
		playerfields.fid_pitch = jni->GetFieldID(playerfields.entity_class, xors("z"), xors("F"));
		playerfields.fid_yaw = jni->GetFieldID(playerfields.entity_class, xors("y"), xors("F"));
		playerfields.fid_prev_yaw = jni->GetFieldID(playerfields.entity_class, xors("A"), xors("F"));
		playerfields.fid_width = jni->GetFieldID(playerfields.entity_class, xors("M"), xors("F"));
		playerfields.fid_height = jni->GetFieldID(playerfields.entity_class, xors("N"), xors("F"));
		playerfields.fid_move_speed = jni->GetStaticFieldID(playerfields.cls_potion, xors("c"), xors("Lrv;"));
		playerfields.fid_step_height = jni->GetFieldID(playerfields.entity_class, xors("W"), xors("F"));
		playerfields.fid_inventory = jni->GetFieldID(playerfields.entity_player_class, xors("bn"), xors("Lzs;"));
		playerfields.fid_inventory_player = jni->GetFieldID(playerfields.entity_player_class, xors("bm"), xors("Lyx;"));
		playerfields.fid_obj_capabilities = jni->GetFieldID(playerfields.entity_player_class, xors("bE"), xors("Lyw;"));
		playerfields.fid_armor_inventory = jni->GetFieldID(playerfields.cls_inventory, xors("b"), xors("[Ladd;"));
		playerfields.fid_aabb = jni->GetFieldID(playerfields.entity_class, xors("C"), xors("Lazt;"));
		playerfields.fid_aabb_min_x = jni->GetFieldID(playerfields.cls_aabb, xors("a"), xors("D"));
		playerfields.fid_aabb_min_y = jni->GetFieldID(playerfields.cls_aabb, xors("b"), xors("D"));
		playerfields.fid_aabb_min_z = jni->GetFieldID(playerfields.cls_aabb, xors("c"), xors("D"));
		playerfields.fid_aabb_max_x = jni->GetFieldID(playerfields.cls_aabb, xors("d"), xors("D"));
		playerfields.fid_aabb_max_y = jni->GetFieldID(playerfields.cls_aabb, xors("e"), xors("D"));
		playerfields.fid_aabb_max_z = jni->GetFieldID(playerfields.cls_aabb, xors("f"), xors("D"));
		playerfields.fid_ticks_existed = jni->GetFieldID(playerfields.entity_player_class, xors("aa"), xors("I"));

		playerfields.mid_get_active_potion_effect = jni->GetMethodID(playerfields.cls_entity_living_base, xors("b"), xors("(Lrv;)Lrw;"));
		playerfields.mid_get_amplifier = jni->GetMethodID(playerfields.cls_potion_effect, xors("c"), xors("()I"));
		playerfields.mid_is_potion_active = jni->GetMethodID(playerfields.cls_entity_living_base, xors("k"), xors("(I)Z"));
		playerfields.mid_is_invisible = jni->GetMethodID(playerfields.entity_class, xors("ap"), xors("()Z"));
		playerfields.mid_is_sneaking = jni->GetMethodID(playerfields.entity_class, xors("an"), xors("()Z"));
		playerfields.mid_is_sprinting = jni->GetMethodID(playerfields.entity_class, xors("ao"), xors("()Z"));
		playerfields.mid_in_water = jni->GetMethodID(playerfields.entity_class, xors("M"), xors("()Z"));
		playerfields.mid_set_sprinting = jni->GetMethodID(playerfields.entity_player_sp_class, xors("c"), xors("(Z)V"));
		playerfields.mid_swing_item = jni->GetMethodID(playerfields.cls_entity_living_base, xors("ba"), xors("()V"));
		playerfields.mid_set_sneaking = jni->GetMethodID(playerfields.entity_class, xors("b"), xors("(Z)V"));
		playerfields.mid_set_velocity = jni->GetMethodID(playerfields.cls_entity_living_base, xors("i"), xors("(DDD)V"));
		playerfields.mid_get_health = jni->GetMethodID(playerfields.cls_entity_living_base, xors("aS"), xors("()F"));
		playerfields.mid_get_max_health = jni->GetMethodID(playerfields.cls_entity_living_base, xors("aY"), xors("()F"));
		playerfields.mid_get_name = jni->GetMethodID(playerfields.entity_player_class, xors("b_"), xors("()Ljava/lang/String;"));
		playerfields.mid_get_held_item = jni->GetMethodID(playerfields.entity_player_class, xors("be"), xors("()Ladd;"));
		playerfields.mid_get_item = jni->GetMethodID(playerfields.item_stack_class, xors("b"), xors("()Ladb;"));
		playerfields.mid_get_slot = jni->GetMethodID(playerfields.cls_container, xors("a"), xors("(I)Laay;"));
		playerfields.mid_get_stack = jni->GetMethodID(playerfields.cls_slot, xors("d"), xors("()Ladd;"));
		playerfields.mid_potion_id = jni->GetMethodID(playerfields.cls_potion_effect, xors("a"), xors("()I"));
		playerfields.mid_get_effects = jni->GetMethodID(playerfields.cls_item_potion, xors("g"), xors("(Ladd;)Ljava/util/List;"));
		playerfields.mid_send_use_item = jni->GetMethodID(playerfields.cls_player_controller, xors("a"), xors("(Lyz;Lahb;Ladd;)Z"));
		playerfields.mid_get_eye_height = jni->GetMethodID(playerfields.entity_class, xors("g"), xors("()F"));
		playerfields.mid_is_visible = jni->GetMethodID(playerfields.cls_entity_living_base, xors("p"), xors("(Lsa;)Z"));
		playerfields.mid_set_position = jni->GetMethodID(playerfields.entity_class, xors("b"), xors("(DDD)V"));
		playerfields.mid_set_angles = jni->GetMethodID(playerfields.entity_class, xors("c"), xors("(FF)V"));

		init_fields = true;
	}

	if (const jobject obj_held = jni->CallObjectMethod(player_instance, playerfields.mid_get_held_item); obj_held != nullptr && ctx.m_ingame)
	{
		if (const jobject obj_held_item = jni->CallObjectMethod(obj_held, playerfields.mid_get_item); obj_held_item != nullptr)
		{
			playerfields.holding_weapon = jni->IsInstanceOf(obj_held_item, playerfields.item_sword_class) || jni->IsInstanceOf(obj_held_item, playerfields.item_axe_class);
			playerfields.holding_projectile = jni->IsInstanceOf(obj_held_item, playerfields.item_egg_class) || jni->IsInstanceOf(obj_held_item, playerfields.item_snowball_class);
			playerfields.holding_block = jni->IsInstanceOf(obj_held_item, playerfields.item_block_class);
		}
	}
	else
	{
		playerfields.holding_weapon = false;
		playerfields.holding_projectile = false;
		playerfields.holding_block = false;
	}
}

bool c_player_1710::is_same(std::shared_ptr<c_player> other)
{
	return jni->IsSameObject(player_instance, other->player_instance);
}

jobject c_player_1710::get_stack(jint slot)
{
	return jni->CallObjectMethod(jni->CallObjectMethod(jni->GetObjectField(player_instance, playerfields.fid_inventory), playerfields.mid_get_slot, slot), playerfields.mid_get_stack);
}

jobject c_player_1710::get_effects(jobject potion, jobject item_stack)
{
	return jni->CallObjectMethod(potion, playerfields.mid_get_effects, item_stack);
}

jint c_player_1710::get_potion_id(jobject effect)
{
	return jni->CallIntMethod(effect, playerfields.mid_potion_id);
}

jint c_player_1710::get_speed_amplifier()
{
	return jni->CallIntMethod(jni->CallObjectMethod(player_instance, playerfields.mid_get_active_potion_effect, jni->GetStaticObjectField(playerfields.cls_potion, playerfields.fid_move_speed)), playerfields.mid_get_amplifier);
}

jboolean c_player_1710::is_invisible()
{
	return jni->CallBooleanMethod(player_instance, playerfields.mid_is_invisible);
}

jboolean c_player_1710::is_sneaking()
{
	return jni->CallBooleanMethod(player_instance, playerfields.mid_is_sneaking);
}

jboolean c_player_1710::is_potion_active(jint potion)
{
	return jni->CallBooleanMethod(player_instance, playerfields.mid_is_potion_active, potion);
}

jboolean c_player_1710::is_sprinting()
{
	return jni->CallBooleanMethod(player_instance, playerfields.mid_is_sprinting);
}

jboolean c_player_1710::is_visible(jobject player)
{
	return jni->CallBooleanMethod(player_instance, playerfields.mid_is_visible, player);
}

jboolean c_player_1710::in_water()
{
	return jni->CallBooleanMethod(player_instance, playerfields.mid_in_water);
}

void c_player_1710::set_sprinting(jboolean sprint)
{
	return jni->CallVoidMethod(player_instance, playerfields.mid_set_sprinting, sprint);
}

jboolean c_player_1710::is_collided_vertically()
{
	return jni->GetBooleanField(player_instance, playerfields.fid_collided_vertically);
}

jboolean c_player_1710::is_collided_horizontally()
{
	return jni->GetBooleanField(player_instance, playerfields.fid_collided_horizontally);
}

jboolean c_player_1710::is_on_ground()
{
	return jni->GetBooleanField(player_instance, playerfields.fid_on_ground);
}

jfloat c_player_1710::get_pitch()
{
	return jni->GetFloatField(player_instance, playerfields.fid_pitch);
}

jfloat c_player_1710::get_yaw()
{
	return jni->GetFloatField(player_instance, playerfields.fid_yaw);
}

jfloat c_player_1710::get_strafing()
{
	return jni->GetFloatField(player_instance, playerfields.fid_move_strafing);
}

jfloat c_player_1710::get_forward()
{
	return jni->GetFloatField(player_instance, playerfields.fid_move_forward);
}

jobject c_player_1710::get_held_item()
{
	return jni->CallObjectMethod(jni->CallObjectMethod(player_instance, playerfields.mid_get_held_item), playerfields.mid_get_item);
}

jobject c_player_1710::get_item(jobject item_stack)
{
	return jni->CallObjectMethod(item_stack, playerfields.mid_get_item);
}

jboolean c_player_1710::holding_weapon()
{
	return playerfields.holding_weapon;
}

jboolean c_player_1710::holding_projectile()
{
	return playerfields.holding_projectile;
}

jboolean c_player_1710::holding_block()
{
	return playerfields.holding_block;
}

jdouble c_player_1710::origin_x()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_xorigin);
}

jdouble c_player_1710::origin_y()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_yorigin);
}

jdouble c_player_1710::origin_z()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_zorigin);
}

jdouble c_player_1710::motion_x()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_xmotion);
}

jdouble c_player_1710::motion_y()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_ymotion);
}

jdouble c_player_1710::motion_z()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_zmotion);
}

jdouble c_player_1710::aabb_min_x()
{
	return jni->GetDoubleField(jni->GetObjectField(player_instance, playerfields.fid_aabb), playerfields.fid_aabb_min_x);
}

jdouble c_player_1710::aabb_min_y()
{
	return jni->GetDoubleField(jni->GetObjectField(player_instance, playerfields.fid_aabb), playerfields.fid_aabb_min_y);
}

jdouble c_player_1710::aabb_min_z()
{
	return jni->GetDoubleField(jni->GetObjectField(player_instance, playerfields.fid_aabb), playerfields.fid_aabb_min_z);
}

jdouble c_player_1710::aabb_max_x()
{
	return jni->GetDoubleField(jni->GetObjectField(player_instance, playerfields.fid_aabb), playerfields.fid_aabb_max_x);
}

jdouble c_player_1710::aabb_max_y()
{
	return jni->GetDoubleField(jni->GetObjectField(player_instance, playerfields.fid_aabb), playerfields.fid_aabb_max_y);
}

jdouble c_player_1710::aabb_max_z()
{
	return jni->GetDoubleField(jni->GetObjectField(player_instance, playerfields.fid_aabb), playerfields.fid_aabb_max_z);
}

jdouble c_player_1710::old_origin_x()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_old_xorigin);
}

jdouble c_player_1710::old_origin_y()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_old_yorigin);
}

jdouble c_player_1710::old_origin_z()
{
	return jni->GetDoubleField(player_instance, playerfields.fid_old_zorigin);
}

jboolean c_player_1710::is_dead()
{
	return jni->GetBooleanField(player_instance, playerfields.fid_is_dead);
}

jfloat c_player_1710::get_width()
{
	return jni->GetFloatField(player_instance, playerfields.fid_width);
}

jfloat c_player_1710::get_health()
{
	return jni->CallFloatMethod(player_instance, playerfields.mid_get_health);
}

jfloat c_player_1710::get_max_health()
{
	return jni->CallFloatMethod(player_instance, playerfields.mid_get_max_health);
}

jboolean c_player_1710::has_armor()
{
	const auto arr_item_stack = static_cast<jobjectArray>(jni->GetObjectField(jni->GetObjectField(player_instance, playerfields.fid_inventory_player), playerfields.fid_armor_inventory));
	if (!arr_item_stack)
		return false;

	for (jint i = 0; i < jni->GetArrayLength(arr_item_stack); i++)
	{
		if (const auto obj = jni->GetObjectArrayElement(arr_item_stack, i); obj != nullptr)
		{
			jni->DeleteLocalRef(arr_item_stack);
			jni->DeleteLocalRef(obj);
			return true;
		}
	}

	jni->DeleteLocalRef(arr_item_stack);
	return false;
}

jfloat c_player_1710::get_height()
{
	return jni->GetFloatField(player_instance, playerfields.fid_height);
}

jfloat c_player_1710::get_eye_height()
{
	return jni->CallFloatMethod(player_instance, playerfields.mid_get_eye_height);
}

jstring c_player_1710::get_name()
{
	return (jstring)jni->CallObjectMethod(player_instance, playerfields.mid_get_name);
}

jint c_player_1710::get_hurt_time()
{
	return jni->GetIntField(player_instance, playerfields.fid_hurt_time);
}

jint c_player_1710::get_max_hurt_time()
{
	return jni->GetIntField(player_instance, playerfields.fid_max_hurt_time);
}

jint c_player_1710::get_ticks_existed()
{
	return jni->GetIntField(player_instance, playerfields.fid_ticks_existed);
}

jint c_player_1710::get_current_slot()
{
	return jni->GetIntField(jni->GetObjectField(player_instance, playerfields.fid_inventory_player), playerfields.fid_current_slot);
}

jboolean c_player_1710::send_use_item(jobject item_stack)
{
	auto mc = ctx.get_game(jni);

	if (!mc)
		return false;

	return jni->CallBooleanMethod(mc->get_player_controller(), playerfields.mid_send_use_item, player_instance, worldfields.obj_world, item_stack);
}

void c_player_1710::set_current_slot(jint slot)
{
	return jni->SetIntField(jni->GetObjectField(player_instance, playerfields.fid_inventory_player), playerfields.fid_current_slot, slot);
}

void c_player_1710::set_motion_x(jdouble motion)
{
	return jni->SetDoubleField(player_instance, playerfields.fid_xmotion, motion);
}

void c_player_1710::set_motion_y(jdouble motion)
{
	return jni->SetDoubleField(player_instance, playerfields.fid_ymotion, motion);
}

void c_player_1710::set_motion_z(jdouble motion)
{
	return jni->SetDoubleField(player_instance, playerfields.fid_zmotion, motion);
}

void c_player_1710::set_step_height(jfloat height)
{
	return jni->SetFloatField(player_instance, playerfields.fid_step_height, height);
}

void c_player_1710::set_pitch(jfloat pitch)
{
	jni->SetFloatField(player_instance, playerfields.fid_pitch, math::clamp(pitch, -90.f, 90.f));
}

void c_player_1710::set_yaw(jfloat yaw)
{
	jni->SetFloatField(player_instance, playerfields.fid_prev_yaw, yaw);
	jni->SetFloatField(player_instance, playerfields.fid_yaw, yaw);
}

void c_player_1710::set_velocity(jdouble x, jdouble y, jdouble z)
{
	jni->CallVoidMethod(player_instance, playerfields.mid_set_velocity, x, y, z);
}

void c_player_1710::set_sneaking(jboolean sneak)
{
	jni->CallVoidMethod(player_instance, playerfields.mid_set_sneaking, sneak);
}

void c_player_1710::set_position(jdouble x, jdouble y, jdouble z)
{
	jni->CallVoidMethod(player_instance, playerfields.mid_set_position, x, y, z);
}

void c_player_1710::set_angles(jfloat yaw, jfloat pitch)
{
	jni->CallVoidMethod(player_instance, playerfields.mid_set_angles, yaw, pitch);
}

void c_game_1710::instantiate(JNIEnv* _jni)
{
	jni = _jni;

	static bool init_fields = false;

	if (!init_fields)
	{
		auto class_loader = ctx.get_class_loader(jni);

		if (!class_loader)
			return;

		gamefields.minecraft = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bao")));
		gamefields.render_manager = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bnn")));
		gamefields.font_renderer = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bbu")));
		gamefields.timer_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bbr")));
		gamefields.entity_renderer_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("blt")));
		gamefields.settings_class = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bbj")));
		gamefields.cls_inventory = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bex")));
		gamefields.cls_chat = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("bct")));
		gamefields.cls_moving_object_position = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("azu")));
		gamefields.cls_lwjgl_sys_impl = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("org/lwjgl/WindowsSysImplementation")));
		gamefields.cls_lwjgl_mouse = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("org/lwjgl/input/Mouse")));
		gamefields.cls_lwjgl_display = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("org/lwjgl/opengl/Display")));

		gamefields.fid_minecraft = jni->GetStaticFieldID(gamefields.minecraft, xors("M"), xors("Lbao;"));
		gamefields.fid_entity_renderer_obj = jni->GetFieldID(gamefields.minecraft, xors("p"), xors("Lblt;"));
		gamefields.fid_timer_obj = jni->GetFieldID(gamefields.minecraft, xors("Q"), xors("Lbbr;"));
		gamefields.fid_game_settings = jni->GetFieldID(gamefields.minecraft, xors("u"), xors("Lbbj;"));
		gamefields.fid_render_manager_obj = jni->GetStaticFieldID(gamefields.render_manager, xors("a"), xors("Lbnn;"));
		gamefields.fid_font_renderer_obj = jni->GetFieldID(gamefields.minecraft, xors("l"), xors("Lbbu;"));
		gamefields.fid_the_player = jni->GetFieldID(gamefields.minecraft, xors("h"), xors("Lbjk;"));
		gamefields.fid_the_world = jni->GetFieldID(gamefields.minecraft, xors("f"), xors("Lbjf;"));
		gamefields.fid_current_screen = jni->GetFieldID(gamefields.minecraft, xors("n"), xors("Lbdw;"));
		gamefields.fid_in_game_has_focus = jni->GetFieldID(gamefields.minecraft, xors("x"), xors("Z"));
		gamefields.fid_right_click_delay = jni->GetFieldID(gamefields.minecraft, xors("ad"), xors("I"));
		gamefields.fid_timer_speed = jni->GetFieldID(gamefields.timer_class, xors("d"), xors("F"));
		gamefields.fid_player_controller = jni->GetFieldID(gamefields.minecraft, xors("c"), xors("Lbje;"));
		gamefields.fid_render_partial_ticks = jni->GetFieldID(gamefields.timer_class, xors("c"), xors("F"));
		gamefields.fid_render_xpos = jni->GetStaticFieldID(gamefields.render_manager, xors("b"), xors("D"));
		gamefields.fid_render_ypos = jni->GetStaticFieldID(gamefields.render_manager, xors("c"), xors("D"));
		gamefields.fid_render_zpos = jni->GetStaticFieldID(gamefields.render_manager, xors("d"), xors("D"));
		gamefields.fid_player_xview = jni->GetFieldID(gamefields.render_manager, xors("k"), xors("F"));
		gamefields.fid_player_yview = jni->GetFieldID(gamefields.render_manager, xors("j"), xors("F"));
		gamefields.fid_pointed_entity = jni->GetFieldID(gamefields.minecraft, xors("j"), xors("Lsa;"));
		gamefields.fid_object_mouse_over = jni->GetFieldID(gamefields.minecraft, xors("t"), xors("Lazu;"));
		gamefields.fid_entity_hit = jni->GetFieldID(gamefields.cls_moving_object_position, xors("g"), xors("Lsa;"));;
		gamefields.fid_view_bobbing = jni->GetFieldID(gamefields.settings_class, xors("d"), xors("Z"));
		gamefields.fid_mouse_sensitivity = jni->GetFieldID(gamefields.settings_class, xors("a"), xors("F"));
		gamefields.fid_type_of_hit = jni->GetFieldID(gamefields.cls_moving_object_position, xors("a"), xors("Lazv;"));

		gamefields.mid_get_minecraft = jni->GetStaticMethodID(gamefields.minecraft, xors("B"), xors("()Lbao;"));
		gamefields.mid_screen_constructor = jni->GetMethodID(gamefields.cls_moving_object_position, xors("<init>"), xors("(Lsa;)V"));
		gamefields.mid_get_string_width = jni->GetMethodID(gamefields.font_renderer, xors("a"), xors("(Ljava/lang/String;)I"));
		gamefields.mid_draw_string_with_shadow = jni->GetMethodID(gamefields.font_renderer, xors("a"), xors("(Ljava/lang/String;IIIZ)I"));
		gamefields.mid_enabled_light_map = jni->GetMethodID(gamefields.entity_renderer_class, xors("b"), xors("(D)V"));
		gamefields.mid_disable_light_map = jni->GetMethodID(gamefields.entity_renderer_class, xors("a"), xors("(D)V"));
		gamefields.mid_setup_camera_transform = jni->GetMethodID(gamefields.entity_renderer_class, xors("a"), xors("(FI)V"));
		gamefields.mid_get_net_handler = jni->GetMethodID(gamefields.minecraft, xors("v"), xors("()Lbjb;"));
		gamefields.mid_set_in_focus = jni->GetMethodID(gamefields.minecraft, xors("l"), xors("()V"));
		gamefields.mid_set_not_in_focus = jni->GetMethodID(gamefields.minecraft, xors("m"), xors("()V"));
		gamefields.mid_get_hwnd = jni->GetStaticMethodID(gamefields.cls_lwjgl_sys_impl, xors("getHwnd"), xors("()J"));
		gamefields.mid_set_cursor_pos = jni->GetStaticMethodID(gamefields.cls_lwjgl_mouse, xors("setCursorPosition"), xors("(II)V"));
		gamefields.mid_get_width = jni->GetStaticMethodID(gamefields.cls_lwjgl_display, xors("getWidth"), xors("()I"));
		gamefields.mid_get_height = jni->GetStaticMethodID(gamefields.cls_lwjgl_display, xors("getHeight"), xors("()I"));
		gamefields.mid_set_mouse_grabbed = jni->GetStaticMethodID(gamefields.cls_lwjgl_mouse, xors("setGrabbed"), xors("(Z)V"));

		gamefields.obj_game = jni->NewGlobalRef(jni->GetStaticObjectField(gamefields.minecraft, gamefields.fid_minecraft));
		gamefields.obj_render_manager = jni->NewGlobalRef(jni->GetStaticObjectField(gamefields.minecraft, gamefields.fid_render_manager_obj));
		gamefields.obj_font_renderer = jni->NewGlobalRef(jni->GetObjectField(gamefields.obj_game, gamefields.fid_font_renderer_obj));
		gamefields.obj_entity_renderer = jni->NewGlobalRef(jni->GetObjectField(gamefields.obj_game, gamefields.fid_entity_renderer_obj));
		gamefields.obj_settings = jni->NewGlobalRef(jni->GetObjectField(gamefields.obj_game, gamefields.fid_game_settings));
		gamefields.obj_timer = jni->NewGlobalRef(jni->GetObjectField(gamefields.obj_game, gamefields.fid_timer_obj));

		init_fields = true;
	}

	if (const jobject obj_screen = jni->GetObjectField(gamefields.obj_game, gamefields.fid_current_screen); obj_screen != nullptr && ctx.m_ingame)
	{
		gamefields.in_inventory = jni->IsInstanceOf(obj_screen, gamefields.cls_inventory);
		gamefields.in_chat = jni->IsInstanceOf(obj_screen, gamefields.cls_chat);
	}
	else
	{
		gamefields.in_inventory = false;
		gamefields.in_chat = false;
	}
}

jboolean c_game_1710::in_game_has_focus()
{
	return jni->GetBooleanField(gamefields.obj_game, gamefields.fid_in_game_has_focus);
}

jboolean c_game_1710::is_in_inventory()
{
	return gamefields.in_inventory;
}

jboolean c_game_1710::is_view_bobbing()
{
	return jni->GetBooleanField(gamefields.obj_settings, gamefields.fid_view_bobbing);
}

jboolean c_game_1710::is_in_chat()
{
	return gamefields.in_chat;
}

jboolean c_game_1710::is_hovering_block()
{
	auto class_loader = ctx.get_class_loader(jni);

	if (!class_loader)
		return false;

	auto type_of_hit = jni->GetObjectField(jni->GetObjectField(gamefields.obj_game, gamefields.fid_object_mouse_over), gamefields.fid_type_of_hit);

	jclass cls_enum = (jclass)jni->NewGlobalRef(class_loader->find_class(xors("java.lang.Enum")));
	jmethodID mid_ordinal = jni->GetMethodID(cls_enum, xors("ordinal"), xors("()I"));

	return jni->CallIntMethod(type_of_hit, mid_ordinal) == 1;
}

void c_game_1710::set_view_bobbing(jboolean val)
{
	jni->SetBooleanField(gamefields.obj_settings, gamefields.fid_view_bobbing, val);
}

void c_game_1710::set_timer_speed(jfloat val)
{
	jni->SetFloatField(gamefields.obj_timer, gamefields.fid_timer_speed, val);
}

void c_game_1710::set_right_click_delay(jint val)
{
	jni->SetIntField(gamefields.obj_game, gamefields.fid_right_click_delay, val);
}

void c_game_1710::set_in_focus()
{
	jni->CallVoidMethod(gamefields.obj_game, gamefields.mid_set_in_focus);
}

void c_game_1710::set_not_in_focus()
{
	jni->CallVoidMethod(gamefields.obj_game, gamefields.mid_set_not_in_focus);
}

std::shared_ptr<c_player> c_game_1710::get_player()
{
	auto player_class = jni->GetObjectField(gamefields.obj_game, gamefields.fid_the_player);
	if (!player_class)
		return nullptr;

	auto player_ptr = std::make_shared<c_player_1710>();
	player_ptr->instantiate(player_class, jni);

	return player_ptr;
}

std::shared_ptr<c_world> c_game_1710::get_world()
{
	auto world_class = jni->GetObjectField(gamefields.obj_game, gamefields.fid_the_world);

	if (!world_class)
		return nullptr;

	auto world_ptr = std::make_shared<c_world_1710>();
	world_ptr->instantiate(world_class, jni);

	return world_ptr;
}

jfloat c_game_1710::get_render_partial_ticks()
{
	return jni->GetFloatField(gamefields.obj_timer, gamefields.fid_render_partial_ticks);
}

jfloat c_game_1710::get_mouse_sensitivity()
{
	return jni->GetFloatField(gamefields.obj_settings, gamefields.fid_mouse_sensitivity);
}

jfloat c_game_1710::get_timer_speed()
{
	return jni->GetFloatField(gamefields.obj_timer, gamefields.fid_timer_speed);
}

jint c_game_1710::get_right_click_delay()
{
	return jni->GetIntField(gamefields.obj_game, gamefields.fid_right_click_delay);
}

jint c_game_1710::get_string_width(jstring string)
{
	return jni->CallIntMethod(gamefields.obj_font_renderer, gamefields.mid_get_string_width, string);
}

void c_game_1710::set_mouse_grabbed(jboolean grabbed)
{
	jni->CallStaticVoidMethod(gamefields.cls_lwjgl_mouse, gamefields.mid_set_mouse_grabbed, grabbed);
}

jint c_game_1710::draw_string_with_shadow(jstring par1, jint par2, jint par3, jint par4)
{
	return jni->CallIntMethod(gamefields.obj_font_renderer, gamefields.mid_draw_string_with_shadow, par1, par2, par3, par4, false);
}

jobject c_game_1710::get_net_handler()
{
	return jni->CallObjectMethod(gamefields.obj_game, gamefields.mid_get_net_handler);
}

jobject c_game_1710::get_player_controller()
{
	return jni->GetObjectField(gamefields.obj_game, gamefields.fid_player_controller);
}

void c_game_1710::enable_light_map()
{
	return jni->CallVoidMethod(gamefields.obj_entity_renderer, gamefields.mid_enabled_light_map, (jdouble)0);
}

void c_game_1710::set_pointed_entity(jobject ent)
{
	return jni->SetObjectField(gamefields.obj_game, gamefields.fid_pointed_entity, ent);
}

void c_game_1710::set_object_mouse_over(jobject player)
{
	if (!player)
		return;

	jni->SetObjectField(gamefields.obj_game, gamefields.fid_object_mouse_over, jni->NewObject(gamefields.cls_moving_object_position, gamefields.mid_screen_constructor, player));
}

void c_game_1710::disable_light_map()
{
	return jni->CallVoidMethod(gamefields.obj_entity_renderer, gamefields.mid_disable_light_map, (jdouble)0);
}

void c_game_1710::setup_camera_transform(jfloat par1, int par2)
{
	return jni->CallVoidMethod(gamefields.obj_entity_renderer, gamefields.mid_setup_camera_transform, par1, par2);
}

jlong c_game_1710::get_window_handle()
{
	return jni->CallStaticLongMethod(gamefields.cls_lwjgl_sys_impl, gamefields.mid_get_hwnd);
}

jint c_game_1710::get_screen_w()
{
	return jni->CallStaticIntMethod(gamefields.cls_lwjgl_display, gamefields.mid_get_width);
}

jint c_game_1710::get_screen_h()
{
	return jni->CallStaticIntMethod(gamefields.cls_lwjgl_display, gamefields.mid_get_height);
}

void c_game_1710::set_cursor_pos(jint x, jint y)
{
	jni->CallStaticVoidMethod(gamefields.cls_lwjgl_mouse, gamefields.mid_set_cursor_pos, x, y);
}
