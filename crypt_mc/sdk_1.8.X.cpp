//#include "context.h"
//#include "sdk_1.8.X.h"
//
//void c_block_18X::instantiate(jobject block_instance, JNIEnv* _jni = nullptr)
//{
//	jni = (_jni) ? _jni : ctx.m_jni;
//
//	auto class_loader = ctx.get_class_loader(jni);
//
//	if (!class_loader)
//		return;
//
//	jclass block_class = class_loader->find_class(xors("net.minecraft.block.Block"));
//	jmethodID id_from_block = jni->GetStaticMethodID(block_class, xors("func_149729_e"), xors("(Lnet/minecraft/block/Block;)I"));
//
//	id = jni->CallStaticIntMethod(block_class, id_from_block, block_instance);
//
//	jni->DeleteLocalRef(block_instance);
//	jni->DeleteLocalRef(block_class);
//}
//
//void c_class_loader_18X::instantiate(JNIEnv* _jni = nullptr)
//{
//	jni = (_jni) ? _jni : ctx.m_jni;
//
//	jclass launch_class = jni->FindClass(xors("net/minecraft/launchwrapper/Launch"));
//	jclass class_loader_class = jni->FindClass(xors("net/minecraft/launchwrapper/LaunchClassLoader"));
//	jfieldID class_loader_field = jni->GetStaticFieldID(launch_class, xors("classLoader"), xors("Lnet/minecraft/launchwrapper/LaunchClassLoader;"));
//
//	//find_class_method = jni->GetMethodID(class_loader_class, xors("findClass"), xors("(Ljava/lang/String;)Ljava/lang/Class;"));
//	//class_loader = jni->GetStaticObjectField(class_loader_class, class_loader_field);
//
//	jni->DeleteLocalRef(launch_class);
//	jni->DeleteLocalRef(class_loader_class);
//}
//
//jclass c_class_loader_18X::find_class(const char* name)
//{
//	jstring java_name = jni->NewStringUTF(name);
//	//jclass clazz = static_cast<jclass>(jni->CallObjectMethod(class_loader, find_class_method, java_name));
//
//	jni->DeleteLocalRef(java_name);
//
//	return nullptr;
//}
//
//struct world_fields
//{
//	jfieldID fid_players = nullptr;
//	jmethodID mid_get_ent = nullptr;
//	jmethodID mid_to_array = nullptr;
//	jmethodID mid_render_world = nullptr;
//};
//
//void c_world_18X::instantiate(jobject world_object, JNIEnv* _jni = nullptr)
//{
//	jni = (_jni) ? _jni : ctx.m_jni;
//
//	auto class_loader = ctx.get_class_loader(jni);
//
//	if (!class_loader)
//		return;
//
//	world_instance = world_object;
//	world_class = class_loader->find_class(xors("net.minecraft.world.World"));
//	entity_render_class = class_loader->find_class(xors("net.minecraft.client.renderer.EntityRenderer"));
//
//	jclass cls_list = class_loader->find_class(xors("java.util.List"));
//
//	static world_fields fields;
//	static bool init_fields = false;
//
//	if (!init_fields)
//	{
//		fields.fid_players = jni->GetFieldID(world_class, xors("field_73010_i"), xors("Ljava/util/List;"));
//		//fields.mid_get_block = jni->GetMethodID(world_class, xors("func_180346_a"), xors("(III)Lnet/minecraft/block/Block;"));
//		fields.mid_get_ent = jni->GetMethodID(world_class, xors("func_73045_a"), xors("(I)Lnet/minecraft/entity/Entity;"));
//		fields.mid_to_array = jni->GetMethodID(cls_list, xors("toArray"), xors("()[Ljava/lang/Object;"));
//		fields.mid_render_world = jni->GetMethodID(entity_render_class, xors("func_78471_a"), xors("(FJ)V"));
//
//		init_fields = true;
//	}
//
//	fid_players = fields.fid_players;
//	//mid_get_block = fields.mid_get_block;
//	mid_get_ent = fields.mid_get_ent;
//	mid_to_array = fields.mid_to_array;
//	mid_render_world = fields.mid_render_world;
//
//	jni->DeleteLocalRef(cls_list);
//	jni->DeleteLocalRef(world_class);
//	jni->DeleteLocalRef(entity_render_class);
//}
//
//std::vector<std::shared_ptr<c_player>> c_world_18X::get_players()
//{
//	std::vector<std::shared_ptr<c_player>> players;
//
//	jobject obj_player_ents = jni->GetObjectField(world_instance, fid_players);
//	jobjectArray arr_player_ents = static_cast<jobjectArray>(jni->CallObjectMethod(obj_player_ents, mid_to_array));
//
//	for (jint i = 0; i < jni->GetArrayLength(arr_player_ents); i++)
//	{
//		auto player_ptr = std::make_shared<c_player_18X>();
//		player_ptr->instantiate(jni->NewGlobalRef(jni->GetObjectArrayElement(arr_player_ents, i)), jni);
//		players.push_back(player_ptr);
//	}
//
//	jni->DeleteLocalRef(arr_player_ents);
//	jni->DeleteLocalRef(obj_player_ents);
//
//	return players;
//}
//
//std::shared_ptr<c_block> c_world_18X::get_block(jint x, jint y, jint z)
//{
//	auto block_class = jni->CallObjectMethod(world_instance, mid_get_block, x, y, z);
//
//	if (!block_class)
//		return nullptr;
//
//	auto block_ptr = std::make_shared<c_block_18X>();
//	block_ptr->instantiate(block_class, jni);
//
//	return block_ptr;
//}
//
//struct player_fields
//{
//	jfieldID fid_max_hurt_time = nullptr;
//	jfieldID fid_hurt_time = nullptr;
//	jfieldID fid_current_slot = nullptr;
//	jfieldID fid_xmotion = nullptr;
//	jfieldID fid_ymotion = nullptr;
//	jfieldID fid_zmotion = nullptr;
//	jfieldID fid_old_xorigin = nullptr;
//	jfieldID fid_old_yorigin = nullptr;
//	jfieldID fid_old_zorigin = nullptr;
//	jfieldID fid_xorigin = nullptr;
//	jfieldID fid_yorigin = nullptr;
//	jfieldID fid_zorigin = nullptr;
//	jfieldID fid_is_dead = nullptr;
//	jfieldID fid_collided_vertically = nullptr;
//	jfieldID fid_collided_horizontally = nullptr;
//	jfieldID fid_move_strafing = nullptr;
//	jfieldID fid_move_forward = nullptr;
//	jfieldID fid_pitch = nullptr;
//	jfieldID fid_yaw = nullptr;
//	jfieldID fid_prev_yaw = nullptr;
//	jfieldID fid_width = nullptr;
//	jfieldID fid_height = nullptr;
//	jfieldID fid_move_speed = nullptr;
//	jfieldID fid_step_height = nullptr;
//	jfieldID fid_inventory = nullptr;
//	jfieldID fid_inventory_player = nullptr;
//	jfieldID fid_obj_capabilities = nullptr;
//
//	jmethodID mid_get_active_potion_effect = nullptr;
//	jmethodID mid_get_amplifier = nullptr;
//	jmethodID mid_is_potion_active = nullptr;
//	jmethodID mid_is_invisible = nullptr;
//	jmethodID mid_is_sneaking = nullptr;
//	jmethodID mid_is_sprinting = nullptr;
//	jmethodID mid_set_sprinting = nullptr;
//	jmethodID mid_swing_item = nullptr;
//	jmethodID mid_set_sneaking = nullptr;
//	jmethodID mid_set_velocity = nullptr;
//	jmethodID mid_get_health = nullptr;
//	jmethodID mid_get_name = nullptr;
//	jmethodID mid_get_held_item = nullptr;
//	jmethodID mid_get_item = nullptr;
//	jmethodID mid_get_slot = nullptr;
//	jmethodID mid_get_stack = nullptr;
//	jmethodID mid_potion_id = nullptr;
//	jmethodID mid_get_effects = nullptr;
//	jmethodID mid_send_use_item = nullptr;
//	jmethodID mid_get_eye_height = nullptr;
//};
//
//void c_player_18X::instantiate(jobject player_object, JNIEnv* _jni = nullptr)
//{
//	jni = (_jni) ? _jni : ctx.m_jni;
//
//	auto class_loader = ctx.get_class_loader(jni);
//
//	if (!class_loader)
//		return;
//
//	player_instance = std::move(player_object);
//
//	jclass entity_class = class_loader->find_class(xors("net.minecraft.entity.Entity"));
//    jclass entity_player_sp_class = class_loader->find_class(xors("net.minecraft.client.entity.EntityPlayerSP"));
//	jclass cls_entity_living_base = class_loader->find_class(xors("net.minecraft.entity.EntityLivingBase"));
//	jclass entity_player_class = class_loader->find_class(xors("net.minecraft.entity.player.EntityPlayer"));
//
//	jclass item_stack_class = class_loader->find_class(xors("net.minecraft.item.ItemStack"));
//	jclass _item_sword_class = class_loader->find_class(xors("net.minecraft.item.ItemSword"));
//	jclass _item_axe_class = class_loader->find_class(xors("net.minecraft.item.ItemAxe"));
//
//	jclass cls_container = class_loader->find_class(xors("net.minecraft.inventory.Container"));
//	jclass cls_slot = class_loader->find_class(xors("net.minecraft.inventory.Slot"));
//    jclass cls_item_potion = class_loader->find_class(xors("net.minecraft.item.ItemPotion"));
//    cls_potion = class_loader->find_class(xors("net.minecraft.potion.Potion"));
//	jclass cls_potion_effect = class_loader->find_class(xors("net.minecraft.potion.PotionEffect"));
//	jclass cls_player_controller = class_loader->find_class(xors("net.minecraft.client.multiplayer.PlayerControllerMP"));
//    jclass cls_inventory = class_loader->find_class(xors("net.minecraft.entity.player.InventoryPlayer"));
//
//	item_sword_class = _item_sword_class;
//	item_axe_class = _item_axe_class;
//
//	static player_fields fields;
//	static bool init_fields = false;
//
//	if (!init_fields)
//	{
//		fields.fid_max_hurt_time = jni->GetFieldID(cls_entity_living_base, xors("field_70738_aO"), xors("I"));
//		fields.fid_hurt_time = jni->GetFieldID(cls_entity_living_base, xors("field_70737_aN"), xors("I"));
//		fields.fid_current_slot = jni->GetFieldID(cls_inventory, xors("field_70461_c"), xors("I"));
//		fields.fid_xmotion = jni->GetFieldID(entity_class, xors("field_70159_w"), xors("D"));
//		fields.fid_ymotion = jni->GetFieldID(entity_class, xors("field_70181_x"), xors("D"));
//		fields.fid_zmotion = jni->GetFieldID(entity_class, xors("field_70179_y"), xors("D"));
//		fields.fid_old_xorigin = jni->GetFieldID(entity_class, xors("field_70142_S"), xors("D"));
//		fields.fid_old_yorigin = jni->GetFieldID(entity_class, xors("field_70137_T"), xors("D"));
//		fields.fid_old_zorigin = jni->GetFieldID(entity_class, xors("field_70136_U"), xors("D"));
//		fields.fid_xorigin = jni->GetFieldID(entity_class, xors("field_70165_t"), xors("D"));
//		fields.fid_yorigin = jni->GetFieldID(entity_class, xors("field_70163_u"), xors("D"));
//		fields.fid_zorigin = jni->GetFieldID(entity_class, xors("field_70161_v"), xors("D"));
//		fields.fid_is_dead = jni->GetFieldID(entity_class, xors("field_70128_L"), xors("Z"));
//		fields.fid_collided_vertically = jni->GetFieldID(entity_class, xors("field_70124_G"), xors("Z"));
//		fields.fid_collided_horizontally = jni->GetFieldID(entity_class, xors("field_70123_F"), xors("Z"));
//		fields.fid_move_strafing = jni->GetFieldID(cls_entity_living_base, xors("field_70702_br"), xors("F"));
//		fields.fid_move_forward = jni->GetFieldID(cls_entity_living_base, xors("field_70701_bs"), xors("F"));
//		fields.fid_pitch = jni->GetFieldID(entity_class, xors("field_70125_A"), xors("F"));
//		fields.fid_yaw = jni->GetFieldID(entity_class, xors("field_70177_z"), xors("F"));
//		fields.fid_prev_yaw = jni->GetFieldID(entity_class, xors("field_70126_B"), xors("F"));
//		fields.fid_width = jni->GetFieldID(entity_class, xors("field_70130_N"), xors("F"));
//		fields.fid_height = jni->GetFieldID(entity_class, xors("field_70131_O"), xors("F"));
//		fields.fid_move_speed = jni->GetStaticFieldID(cls_potion, xors("field_76424_c"), xors("Lnet/minecraft/potion/Potion;"));
//		fields.fid_step_height = jni->GetFieldID(entity_class, xors("field_70138_W"), xors("F"));
//		fields.fid_inventory = jni->GetFieldID(entity_player_class, xors("field_71069_bz"), xors("Lnet/minecraft/inventory/Container;"));
//		fields.fid_inventory_player = jni->GetFieldID(entity_player_class, xors("field_71071_by"), xors("Lnet/minecraft/entity/player/InventoryPlayer;"));
//		fields.fid_obj_capabilities = jni->GetFieldID(entity_player_class, xors("field_71075_bZ"), xors("Lnet/minecraft/entity/player/PlayerCapabilities;"));
//
//		fields.mid_get_active_potion_effect = jni->GetMethodID(cls_entity_living_base, xors("func_70660_b"), xors("(Lnet/minecraft/potion/Potion;)Lnet/minecraft/potion/PotionEffect;"));
//		fields.mid_get_amplifier = jni->GetMethodID(cls_potion_effect, xors("func_76458_c"), xors("()I"));
//		fields.mid_is_potion_active = jni->GetMethodID(cls_entity_living_base, xors("func_82165_m"), xors("(I)Z"));
//		fields.mid_is_invisible = jni->GetMethodID(entity_class, xors("func_82150_aj"), xors("()Z"));
//		fields.mid_is_sneaking = jni->GetMethodID(entity_class, xors("func_70093_af"), xors("()Z"));
//		fields.mid_is_sprinting = jni->GetMethodID(entity_class, xors("func_70051_ag"), xors("()Z"));
//		fields.mid_set_sprinting = jni->GetMethodID(entity_player_sp_class, xors("func_70031_b"), xors("(Z)V"));
//		fields.mid_swing_item = jni->GetMethodID(cls_entity_living_base, xors("func_71038_i"), xors("()V"));
//		fields.mid_set_sneaking = jni->GetMethodID(entity_class, xors("func_70095_a"), xors("(Z)V"));
//		fields.mid_set_velocity = jni->GetMethodID(cls_entity_living_base, xors("func_70016_h"), xors("(DDD)V"));
//		fields.mid_get_health = jni->GetMethodID(cls_entity_living_base, xors("func_110143_aJ"), xors("()F"));
//		fields.mid_get_name = jni->GetMethodID(entity_player_class, xors("func_70005_c_"), xors("()Ljava/lang/String;"));
//		fields.mid_get_held_item = jni->GetMethodID(entity_player_class, xors("func_70694_bm"), xors("()Lnet/minecraft/item/ItemStack;"));
//		fields.mid_get_item = jni->GetMethodID(item_stack_class, xors("func_77973_b"), xors("()Lnet/minecraft/item/Item;"));
//		fields.mid_get_slot = jni->GetMethodID(cls_container, xors("func_75139_a"), xors("(I)Lnet/minecraft/inventory/Slot;"));
//		fields.mid_get_stack = jni->GetMethodID(cls_slot, xors("func_75211_c"), xors("()Lnet/minecraft/item/ItemStack;"));
//		fields.mid_potion_id = jni->GetMethodID(cls_potion_effect, xors("func_76456_a"), xors("()I"));
//		fields.mid_get_effects = jni->GetMethodID(cls_item_potion, xors("func_77832_l"), xors("(Lnet/minecraft/item/ItemStack;)Ljava/util/List;"));
//		fields.mid_send_use_item = jni->GetMethodID(cls_player_controller, xors("func_78769_a"), xors("(Lnet/minecraft/entity/player/EntityPlayer;Lnet/minecraft/world/World;Lnet/minecraft/item/ItemStack;)Z"));
//		fields.mid_get_eye_height = jni->GetMethodID(entity_class, xors("func_70047_e"), xors("()F"));
//
//		init_fields = true;
//	}
//
//	fid_max_hurt_time = fields.fid_max_hurt_time;
//	fid_hurt_time = fields.fid_hurt_time;
//	fid_current_slot = fields.fid_current_slot;
//	fid_xmotion = fields.fid_xmotion;
//	fid_ymotion = fields.fid_ymotion;
//	fid_zmotion = fields.fid_zmotion;
//	fid_old_xorigin = fields.fid_old_xorigin;
//	fid_old_yorigin = fields.fid_old_yorigin;
//	fid_old_zorigin = fields.fid_old_zorigin;
//	fid_xorigin = fields.fid_xorigin;
//	fid_yorigin = fields.fid_yorigin;
//	fid_zorigin = fields.fid_zorigin;
//	fid_is_dead = fields.fid_is_dead;
//	fid_collided_vertically = fields.fid_collided_vertically;
//	fid_collided_horizontally = fields.fid_collided_horizontally;
//	fid_move_strafing = fields.fid_move_strafing;
//	fid_move_forward = fields.fid_move_forward;
//	fid_pitch = fields.fid_pitch;
//	fid_yaw = fields.fid_yaw;
//	fid_prev_yaw = fields.fid_prev_yaw;
//	fid_width = fields.fid_width;
//	fid_height = fields.fid_height;
//	fid_move_speed = fields.fid_move_speed;
//	fid_step_height = fields.fid_step_height;
//	fid_inventory = fields.fid_inventory;
//	fid_inventory_player = fields.fid_inventory_player;
//	fid_obj_capabilities = fields.fid_obj_capabilities;
//
//	mid_get_active_potion_effect = fields.mid_get_active_potion_effect;
//	mid_get_amplifier = fields.mid_get_amplifier;
//	mid_is_potion_active = fields.mid_is_potion_active;
//	mid_is_invisible = fields.mid_is_invisible;
//	mid_is_sneaking = fields.mid_is_sneaking;
//	mid_is_sprinting = fields.mid_is_sprinting;
//	mid_set_sprinting = fields.mid_set_sprinting;
//	mid_swing_item = fields.mid_swing_item;
//	mid_set_sneaking = fields.mid_set_sneaking;
//	mid_set_velocity = fields.mid_set_velocity;
//	mid_get_health = fields.mid_get_health;
//	mid_get_name = fields.mid_get_name;
//	mid_get_held_item = fields.mid_get_held_item;
//	mid_get_item = fields.mid_get_item;
//	mid_get_slot = fields.mid_get_slot;
//	mid_get_stack = fields.mid_get_stack;
//	mid_potion_id = fields.mid_potion_id;
//	mid_get_effects = fields.mid_get_effects;
//	mid_send_use_item = fields.mid_send_use_item;
//	mid_get_eye_height = fields.mid_get_eye_height;
//
//    jni->DeleteLocalRef(entity_class);
//    jni->DeleteLocalRef(entity_player_sp_class);
//    jni->DeleteLocalRef(cls_entity_living_base);
//    jni->DeleteLocalRef(entity_player_class);
//    jni->DeleteLocalRef(item_stack_class);
//    jni->DeleteLocalRef(_item_sword_class);
//    jni->DeleteLocalRef(_item_axe_class);
//    jni->DeleteLocalRef(cls_container);
//    jni->DeleteLocalRef(cls_slot);
//    jni->DeleteLocalRef(cls_item_potion);
//    jni->DeleteLocalRef(cls_potion_effect);
//    jni->DeleteLocalRef(cls_player_controller);
//    jni->DeleteLocalRef(cls_inventory);
//}
//
//bool c_player_18X::is_same(std::shared_ptr<c_player> other)
//{
//	return jni->IsSameObject(player_instance, other->player_instance);
//}
//
//jobject c_player_18X::get_stack(jint slot)
//{
//	return jni->CallObjectMethod(jni->CallObjectMethod(jni->GetObjectField(player_instance, fid_inventory), mid_get_slot, slot), mid_get_stack);
//}
//
//jobject c_player_18X::get_effects(jobject potion, jobject item_stack)
//{
//	return jni->CallObjectMethod(potion, mid_get_effects, item_stack);
//}
//
//jint c_player_18X::get_potion_id(jobject effect)
//{
//	return jni->CallIntMethod(effect, mid_potion_id);
//}
//
//jint c_player_18X::get_speed_amplifier()
//{
//    return jni->CallIntMethod(jni->CallObjectMethod(player_instance, mid_get_active_potion_effect, jni->GetStaticObjectField(cls_potion, fid_move_speed)), mid_get_amplifier);
//}
//
//jboolean c_player_18X::is_invisible()
//{
//	return jni->CallBooleanMethod(player_instance, mid_is_invisible);
//}
//
//jboolean c_player_18X::is_collided_vertically()
//{
//    return jni->GetBooleanField(player_instance, fid_collided_vertically);
//}
//
//jboolean c_player_18X::is_collided_horizontally()
//{
//    return jni->GetBooleanField(player_instance, fid_collided_horizontally);
//}
//
//jboolean c_player_18X::is_sneaking()
//{
//	return jni->CallBooleanMethod(player_instance, mid_is_sneaking);
//}
//
//jboolean c_player_18X::is_sprinting()
//{
//    return jni->CallBooleanMethod(player_instance, mid_is_sprinting);
//}
//
//void c_player_18X::set_sprinting(jboolean sprint)
//{
//    return jni->CallVoidMethod(player_instance, mid_set_sprinting, sprint);
//}
//
//jboolean c_player_18X::is_potion_active(jint potion) 
//{
//    return jni->CallBooleanMethod(player_instance, mid_is_potion_active, potion);
//}
//
//jfloat c_player_18X::get_pitch()
//{
//	return jni->GetFloatField(player_instance, fid_pitch);
//}
//
//jfloat c_player_18X::get_yaw()
//{
//	return jni->GetFloatField(player_instance, fid_yaw);
//}
//
//jfloat c_player_18X::get_strafing()
//{
//	return jni->GetFloatField(player_instance, fid_move_strafing);
//}
//
//jfloat c_player_18X::get_forward()
//{
//	return jni->GetFloatField(player_instance, fid_move_forward);
//}
//
//
//jfloat c_player_18X::get_eye_height()
//{
//    return jni->CallFloatMethod(player_instance, mid_get_eye_height);
//}
//
//jobject c_player_18X::get_held_item()
//{
//	return jni->CallObjectMethod(jni->CallObjectMethod(player_instance, mid_get_held_item), mid_get_item);
//}
//
//jobject c_player_18X::get_item(jobject item_stack)
//{
//	return jni->CallObjectMethod(item_stack, mid_get_item);
//}
//
//jboolean c_player_18X::holding_weapon()
//{
//	if (!this->get_held_item())
//		return false;
//
//	return jni->IsInstanceOf(this->get_held_item(), item_sword_class) || jni->IsInstanceOf(this->get_held_item(), item_axe_class);
//}
//
//jdouble c_player_18X::origin_x()
//{
//	return jni->GetDoubleField(player_instance, fid_xorigin);
//}
//
//jdouble c_player_18X::origin_y()
//{
//	return jni->GetDoubleField(player_instance, fid_yorigin);
//}
//
//jfloat c_player_18X::get_health()
//{
//    return jni->CallFloatMethod(player_instance, mid_get_health);
//}
//
//jdouble c_player_18X::origin_z()
//{
//	return jni->GetDoubleField(player_instance, fid_zorigin);
//}
//
//jdouble c_player_18X::motion_x()
//{
//	return jni->GetDoubleField(player_instance, fid_xmotion);
//}
//
//jdouble c_player_18X::motion_y()
//{
//	return jni->GetDoubleField(player_instance, fid_ymotion);
//}
//
//jdouble c_player_18X::motion_z()
//{
//	return jni->GetDoubleField(player_instance, fid_zmotion);
//}
//
//jdouble c_player_18X::old_origin_x()
//{
//	return jni->GetDoubleField(player_instance, fid_old_xorigin);
//}
//
//jdouble c_player_18X::old_origin_y()
//{
//	return jni->GetDoubleField(player_instance, fid_old_yorigin);
//}
//
//jdouble c_player_18X::old_origin_z()
//{
//	return jni->GetDoubleField(player_instance, fid_old_zorigin);
//}
//
//jfloat c_player_18X::health()
//{
//	return jni->CallFloatMethod(player_instance, mid_get_health);
//}
//
//jboolean c_player_18X::is_dead()
//{
//	return jni->GetBooleanField(player_instance, fid_is_dead);
//}
//
//jfloat c_player_18X::get_width()
//{
//	return jni->GetFloatField(player_instance, fid_width);
//}
//
//jfloat c_player_18X::get_height()
//{
//	return jni->GetFloatField(player_instance, fid_height);
//}
//
//jboolean c_player_18X::send_use_item(jobject item_stack)
//{
//	auto class_loader = ctx.get_class_loader(jni);
//	auto mc = ctx.get_game(jni);
//
//	if (!class_loader || !mc)
//		return false;
//
//	jclass minecraft_class = class_loader->find_class(xors("net.minecraft.client.Minecraft"));
//
//	jfieldID fid_minecraft = jni->GetStaticFieldID(minecraft_class, xors("field_71432_P"), xors("Lnet/minecraft/client/Minecraft;"));
//	jfieldID fid_the_world = jni->GetFieldID(minecraft_class, xors("field_71441_e"), xors("Lnet/minecraft/client/multiplayer/WorldClient;"));
//
//	jobject obj_game = jni->GetStaticObjectField(minecraft_class, fid_minecraft);
//	jobject obj_world = jni->GetObjectField(obj_game, fid_the_world);
//
//	return jni->CallBooleanMethod(mc->get_player_controller(), mid_send_use_item, player_instance, obj_world, item_stack);
//}
//
//jstring c_player_18X::get_name()
//{
//	return (jstring)jni->CallObjectMethod(player_instance, mid_get_name);
//}
//
//jint c_player_18X::get_hurt_time()
//{
//	return jni->GetIntField(player_instance, fid_hurt_time);
//}
//
//jint c_player_18X::get_max_hurt_time()
//{
//	return jni->GetIntField(player_instance, fid_max_hurt_time);
//}
//
//jint c_player_18X::get_current_slot()
//{
//	return jni->GetIntField(jni->GetObjectField(player_instance, fid_inventory_player), fid_current_slot);
//}
//
//void c_player_18X::set_current_slot(jint slot)
//{
//	return jni->SetIntField(jni->GetObjectField(player_instance, fid_inventory_player), fid_current_slot, slot);
//}
//
//void c_player_18X::set_motion_x(jdouble motion)
//{
//	return jni->SetDoubleField(player_instance, fid_xmotion, motion);
//}
//
//void c_player_18X::set_motion_y(jdouble motion)
//{
//	return jni->SetDoubleField(player_instance, fid_ymotion, motion);
//}
//
//void c_player_18X::set_motion_z(jdouble motion)
//{
//	return jni->SetDoubleField(player_instance, fid_zmotion, motion);
//}
//
//void c_player_18X::set_step_height(jfloat height)
//{
//    return jni->SetFloatField(player_instance, fid_step_height, height);
//}
//
//void c_player_18X::set_pitch(jfloat pitch)
//{
//	jni->SetFloatField(player_instance, fid_pitch, math::clamp(pitch, -90.f, 90.f));
//}
//
//void c_player_18X::set_yaw(jfloat yaw)
//{
//	jni->SetFloatField(player_instance, fid_prev_yaw, yaw);
//	jni->SetFloatField(player_instance, fid_yaw, yaw);
//}
//
//void c_player_18X::set_velocity(jdouble x, jdouble y, jdouble z)
//{
//	jni->CallVoidMethod(player_instance, mid_set_velocity, x, y, z);
//}
//
//void c_player_18X::set_sneaking(jboolean sneak)
//{
//	jni->CallVoidMethod(player_instance, mid_set_sneaking, sneak);
//}
//
//struct game_fields
//{
//	jfieldID fid_the_player = nullptr;
//	jfieldID fid_the_world = nullptr;
//	jfieldID fid_current_screen = nullptr;
//	jfieldID fid_in_game_has_focus = nullptr;
//	jfieldID fid_right_click_delay = nullptr;
//	jfieldID fid_timer_speed = nullptr;
//	jfieldID fid_player_controller = nullptr;
//	jfieldID fid_render_partial_ticks = nullptr;
//	jfieldID fid_render_xpos = nullptr;
//	jfieldID fid_render_ypos = nullptr;
//	jfieldID fid_render_zpos = nullptr;
//	jfieldID fid_player_xview = nullptr;
//	jfieldID fid_player_yview = nullptr;
//	jfieldID fid_pointed_entity = nullptr;
//	jfieldID fid_object_mouse_over = nullptr;
//	jfieldID fid_entity_hit = nullptr;
//	jfieldID fid_view_bobbing = nullptr;
//	jfieldID fid_mouse_sensitivity = nullptr;
//
//	jmethodID mid_get_string_width = nullptr;
//	jmethodID mid_draw_string_with_shadow = nullptr;
//	jmethodID mid_enabled_light_map = nullptr;
//	jmethodID mid_disable_light_map = nullptr;
//	jmethodID mid_setup_camera_transform = nullptr;
//	jmethodID mid_get_net_handler = nullptr;
//};
//
//void c_game_18X::instantiate(JNIEnv* _jni = nullptr)
//{
//	jni = (_jni) ? _jni : ctx.m_jni;
//
//	auto class_loader = ctx.get_class_loader(jni);
//
//	if (!class_loader)
//		return;
//
//	jclass minecraft_class = class_loader->find_class(xors("net.minecraft.client.Minecraft"));
//
//	static jfieldID fid_minecraft;
//	static jfieldID fid_entity_renderer_obj;
//	static jfieldID fid_timer_obj;
//	static jfieldID fid_game_settings;
//	static jfieldID fid_render_manager_obj;
//	static jfieldID fid_font_renderer_obj;
//	static game_fields fields;
//	static bool init_fields = false;
//
//	render_manager = class_loader->find_class(xors("net.minecraft.client.renderer.entity.RenderManager"));
//	font_renderer = class_loader->find_class(xors("net.minecraft.client.gui.FontRenderer"));
//	timer_class = class_loader->find_class(xors("net.minecraft.util.Timer"));
//	entity_renderer_class = class_loader->find_class(xors("net.minecraft.client.renderer.EntityRenderer"));
//	settings_class = class_loader->find_class(xors("net.minecraft.client.settings.GameSettings"));
//	cls_inventory = class_loader->find_class(xors("net.minecraft.client.gui.inventory.GuiContainer"));
//	cls_chat = class_loader->find_class(xors("net.minecraft.client.gui.GuiChat"));
//	cls_moving_object_position = class_loader->find_class(xors("net.minecraft.util.MovingObjectPosition"));
//
//	if (!init_fields)
//	{
//		fid_minecraft = jni->GetStaticFieldID(minecraft_class, xors("field_71432_P"), xors("Lnet/minecraft/client/Minecraft;"));
//		fid_entity_renderer_obj = jni->GetFieldID(minecraft_class, xors("field_71460_t"), xors("Lnet/minecraft/client/renderer/EntityRenderer;"));
//		fid_timer_obj = jni->GetFieldID(minecraft_class, xors("field_71428_T"), xors("Lnet/minecraft/util/Timer;"));
//		fid_game_settings = jni->GetFieldID(minecraft_class, xors("field_71474_y"), xors("Lnet/minecraft/client/settings/GameSettings;"));
//
//		//func_175598_ae or func_177068_d
//		fid_render_manager_obj = jni->GetFieldID(minecraft_class, xors("field_175616_W"), xors("Lnet/minecraft/client/renderer/entity/RenderManager;"));
//		fid_font_renderer_obj = jni->GetFieldID(minecraft_class, xors("field_71466_p"), xors("Lnet/minecraft/client/gui/FontRenderer;"));
//
//		fields.fid_the_player = jni->GetFieldID(minecraft_class, xors("field_71439_g"), xors("Lnet/minecraft/client/entity/EntityPlayerSP;"));
//		fields.fid_the_world = jni->GetFieldID(minecraft_class, xors("field_71441_e"), xors("Lnet/minecraft/client/multiplayer/WorldClient;"));
//		fields.fid_current_screen = jni->GetFieldID(minecraft_class, xors("field_71462_r"), xors("Lnet/minecraft/client/gui/GuiScreen;"));
//		fields.fid_in_game_has_focus = jni->GetFieldID(minecraft_class, xors("field_71415_G"), xors("Z"));
//		fields.fid_right_click_delay = jni->GetFieldID(minecraft_class, xors("field_71467_ac"), xors("I"));
//		fields.fid_timer_speed = jni->GetFieldID(timer_class, xors("field_74278_d"), xors("F"));
//		fields.fid_player_controller = jni->GetFieldID(minecraft_class, xors("field_71442_b"), xors("Lnet/minecraft/client/multiplayer/PlayerControllerMP;"));
//		fields.fid_render_partial_ticks = jni->GetFieldID(timer_class, xors("field_74281_c"), xors("F"));
//		fields.fid_render_xpos = jni->GetFieldID(render_manager, xors("field_78725_b"), xors("D"));
//		fields.fid_render_ypos = jni->GetFieldID(render_manager, xors("field_78726_c"), xors("D"));
//		fields.fid_render_zpos = jni->GetFieldID(render_manager, xors("field_78723_d"), xors("D"));
//		fields.fid_player_xview = jni->GetFieldID(render_manager, xors("field_78732_j"), xors("F"));
//		fields.fid_player_yview = jni->GetFieldID(render_manager, xors("field_78735_i"), xors("F"));
//		fields.fid_pointed_entity = jni->GetFieldID(minecraft_class, xors("field_147125_j"), xors("Lnet/minecraft/entity/Entity;"));
//		fields.fid_object_mouse_over = jni->GetFieldID(minecraft_class, xors("field_71476_x"), xors("Lnet/minecraft/util/MovingObjectPosition;"));
//		fields.fid_entity_hit = jni->GetFieldID(cls_moving_object_position, xors("field_72308_g"), xors("Lnet/minecraft/entity/Entity;"));
//		fields.fid_view_bobbing = jni->GetFieldID(settings_class, xors("field_74336_f"), xors("Z"));
//		fields.fid_mouse_sensitivity = jni->GetFieldID(settings_class, xors("field_74341_c"), xors("F"));
//
//		fields.mid_get_string_width = jni->GetMethodID(font_renderer, xors("func_78256_a"), xors("(Ljava/lang/String;)I"));
//		fields.mid_draw_string_with_shadow = jni->GetMethodID(font_renderer, xors("func_175063_a"), xors("(Ljava/lang/String;FFI)I"));
//		fields.mid_enabled_light_map = jni->GetMethodID(entity_renderer_class, xors("func_180436_i"), xors("()V"));
//		fields.mid_disable_light_map = jni->GetMethodID(entity_renderer_class, xors("func_175072_h"), xors("()V"));
//		fields.mid_setup_camera_transform = jni->GetMethodID(entity_renderer_class, xors("func_78479_a"), xors("(FI)V"));
//		fields.mid_get_net_handler = jni->GetMethodID(minecraft_class, xors("func_147114_u"), xors("()Lnet/minecraft/client/network/NetHandlerPlayClient;"));
//
//		init_fields = true;
//	}
//
//	obj_game = jni->GetStaticObjectField(minecraft_class, fid_minecraft);
//	obj_render_manager = jni->GetObjectField(obj_game, fid_render_manager_obj);
//	obj_font_renderer = jni->GetObjectField(obj_game, fid_font_renderer_obj);
//	obj_entity_renderer = jni->GetObjectField(obj_game, fid_entity_renderer_obj);
//	obj_settings = jni->GetObjectField(obj_game, fid_game_settings);
//	obj_timer = jni->GetObjectField(obj_game, fid_timer_obj);
//
//	fid_the_player = fields.fid_the_player;
//	fid_the_world = fields.fid_the_world;
//	fid_current_screen = fields.fid_current_screen;
//	fid_in_game_has_focus = fields.fid_in_game_has_focus;
//	fid_right_click_delay = fields.fid_right_click_delay;
//	fid_timer_speed = fields.fid_timer_speed;
//	fid_player_controller = fields.fid_player_controller;
//	fid_render_partial_ticks = fields.fid_render_partial_ticks;
//	fid_render_xpos = fields.fid_render_xpos;
//	fid_render_ypos = fields.fid_render_ypos;
//	fid_render_zpos = fields.fid_render_zpos;
//	fid_player_xview = fields.fid_player_xview;
//	fid_player_yview = fields.fid_player_yview;
//	fid_pointed_entity = fields.fid_pointed_entity;
//	fid_object_mouse_over = fields.fid_object_mouse_over;
//	fid_entity_hit = fields.fid_entity_hit;
//	fid_view_bobbing = fields.fid_view_bobbing;
//	fid_mouse_sensitivity = fields.fid_mouse_sensitivity;
//
//	mid_get_string_width = fields.mid_get_string_width;
//	mid_draw_string_with_shadow = fields.mid_draw_string_with_shadow;
//	mid_enabled_light_map = fields.mid_enabled_light_map;
//	mid_disable_light_map = fields.mid_disable_light_map;
//	mid_setup_camera_transform = fields.mid_setup_camera_transform;
//	mid_get_net_handler = fields.mid_get_net_handler;
//
//    jni->DeleteLocalRef(minecraft_class);
//    jni->DeleteLocalRef(timer_class);
//    jni->DeleteLocalRef(font_renderer);
//    jni->DeleteLocalRef(render_manager);
//    jni->DeleteLocalRef(settings_class);
//    jni->DeleteLocalRef(entity_renderer_class);
//}
//
//jboolean c_game_18X::in_game_has_focus()
//{
//	return jni->GetBooleanField(obj_game, fid_in_game_has_focus);
//}
//
//jboolean c_game_18X::is_in_inventory()
//{
//	jobject obj_screen = jni->GetObjectField(obj_game, fid_current_screen);
//	if (!obj_screen)
//		return false;
//
//	return jni->IsInstanceOf(obj_screen, cls_inventory);
//}
//
//jboolean c_game_18X::is_view_bobbing()
//{
//	return jni->GetBooleanField(obj_settings, fid_view_bobbing);
//}
//
//jboolean c_game_18X::is_in_chat()
//{
//	jobject obj_screen = jni->GetObjectField(obj_game, fid_current_screen);
//	if (!obj_screen)
//		return false;
//
//	return jni->IsInstanceOf(obj_screen, cls_chat);
//}
//
//void c_game_18X::set_view_bobbing(jboolean val)
//{
//	jni->SetBooleanField(obj_settings, fid_view_bobbing, val);
//}
//
//void c_game_18X::set_timer_speed(jfloat val)
//{
//	jni->SetFloatField(obj_timer, fid_timer_speed, val);
//}
//
//void c_game_18X::set_right_click_delay(jint val)
//{
//	jni->SetIntField(obj_game, fid_right_click_delay, val);
//}
//
//void c_game_18X::set_pointed_entity(jobject ent)
//{
//	return jni->SetObjectField(obj_game, fid_pointed_entity, ent);
//}
//
//void c_game_18X::set_object_mouse_over(jobject player)
//{
//    jmethodID mid_constructor = jni->GetMethodID(cls_moving_object_position, "<init>", "(Lnet/minecraft/entity/Entity;)V");
//
//    if (!mid_constructor || !player)
//        return;
//
//    jobject obj_new_mouse_over = jni->NewObject(cls_moving_object_position, mid_constructor, player);
//
//    return jni->SetObjectField(obj_game, this->fid_object_mouse_over, obj_new_mouse_over);
//}
//
//std::shared_ptr<c_player> c_game_18X::get_player()
//{
//	auto player_class = jni->GetObjectField(obj_game, fid_the_player);
//
//	if (!player_class)
//		return nullptr;
//
//	auto player_ptr = std::make_shared<c_player_18X>();
//	player_ptr->instantiate(player_class, jni);
//
//	return player_ptr;
//}
//
//std::shared_ptr<c_world> c_game_18X::get_world()
//{
//	auto world_class = jni->GetObjectField(obj_game, fid_the_world);
//
//	if (!world_class)
//		return nullptr;
//
//	auto world_ptr = std::make_shared<c_world_18X>();
//	world_ptr->instantiate(world_class, jni);
//
//	return world_ptr;
//}
//
//jdouble c_game_18X::get_render_xpos()
//{
//	return jni->GetDoubleField(render_manager, fid_render_xpos);
//}
//
//jdouble c_game_18X::get_render_ypos()
//{
//	return jni->GetDoubleField(render_manager, fid_render_ypos);
//}
//
//jdouble c_game_18X::get_render_zpos()
//{
//	return jni->GetDoubleField(render_manager, fid_render_zpos);
//}
//
//jfloat c_game_18X::get_player_xview()
//{
//	return jni->GetFloatField(obj_render_manager, fid_player_xview);
//}
//
//jfloat c_game_18X::get_player_yview()
//{
//	return jni->GetFloatField(obj_render_manager, fid_player_yview);
//}
//
//jfloat c_game_18X::get_render_partial_ticks()
//{
//	return jni->GetFloatField(obj_timer, fid_render_partial_ticks);
//}
//
//jfloat c_game_18X::get_mouse_sensitivity()
//{
//	return jni->GetFloatField(obj_settings, fid_mouse_sensitivity);
//}
//
//jfloat c_game_18X::get_timer_speed()
//{
//	return jni->GetFloatField(obj_timer, fid_timer_speed);
//}
//
//jobject c_game_18X::get_player_controller()
//{
//	return jni->GetObjectField(obj_game, fid_player_controller);
//}
//
//jint c_game_18X::get_right_click_delay()
//{
//	return jni->GetIntField(obj_game, fid_right_click_delay);
//}
//
//jint c_game_18X::get_string_width(jstring string)
//{
//	return jni->CallIntMethod(obj_font_renderer, mid_get_string_width, string);
//}
//
//jint c_game_18X::draw_string_with_shadow(jstring par1, jint par2, jint par3, jint par4)
//{
//	return jni->CallIntMethod(obj_font_renderer, mid_draw_string_with_shadow, par1, par2, par3, par4, false);
//}
//
//jobject c_game_18X::get_net_handler()
//{
//	return jni->CallObjectMethod(obj_game, mid_get_net_handler);
//}
//
//void c_game_18X::enable_light_map()
//{
//	return jni->CallVoidMethod(obj_entity_renderer, mid_enabled_light_map);
//}
//
//void c_game_18X::disable_light_map()
//{
//	return jni->CallVoidMethod(obj_entity_renderer, mid_disable_light_map);
//}
//
//void c_game_18X::setup_camera_transform(jfloat par1, int par2)
//{
//	return jni->CallVoidMethod(obj_entity_renderer, mid_setup_camera_transform, par1, par2);
//}
