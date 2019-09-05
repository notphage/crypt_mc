#pragma once

#include "sdk.h"

class c_block_1710 : public c_block
{
public:
	void instantiate(jobject, JNIEnv*) override;
};

class c_class_loader_1710 : public c_class_loader
{
public:
	void instantiate(JNIEnv*) override;
	jclass find_class(const char*) override;
};

class c_player_1710 : public c_player
{
public:
	void instantiate(jobject, JNIEnv*) override;

	bool is_same(std::shared_ptr<c_player>) override;
	jboolean is_invisible() override;
	jboolean is_sneaking() override;
	jboolean is_potion_active(jint) override;

	jfloat get_pitch() override;
	jfloat get_yaw() override;
	jfloat get_strafing() override;
	jfloat get_forward() override;
	jobject get_held_item() override;
	jboolean holding_weapon() override;
	jboolean holding_projectile() override;
	jboolean holding_block() override;

	jdouble origin_x() override;
	jdouble origin_y() override;
	jdouble origin_z() override;

	jdouble motion_x() override;
	jdouble motion_y() override;
	jdouble motion_z() override;

	jdouble aabb_min_x() override;
	jdouble aabb_min_y() override;
	jdouble aabb_min_z() override;
	jdouble aabb_max_x() override;
	jdouble aabb_max_y() override;
	jdouble aabb_max_z() override;

	jdouble old_origin_x() override;
	jdouble old_origin_y() override;
	jdouble old_origin_z() override;

	jboolean is_dead() override;
	jboolean is_collided_vertically() override;
	jboolean is_collided_horizontally() override;
	jboolean is_on_ground() override;
	jboolean is_sprinting() override;
	jboolean is_visible(jobject) override;
	jboolean in_water() override;

	jfloat get_width() override;
	jfloat get_height() override;
	jfloat get_eye_height() override;
	jfloat get_health() override;
	jfloat get_max_health() override;
	jboolean has_armor() override;

	jstring get_name() override;

	jint get_hurt_time() override;
	jint get_max_hurt_time() override;
	jint get_ticks_existed() override;

	jobject get_stack(jint) override;
	jobject get_effects(jobject, jobject) override;
	jint get_potion_id(jobject) override;
	jint get_speed_amplifier();
	jint get_current_slot() override;
	jboolean send_use_item(jobject) override;
	jobject get_item(jobject) override;

	void set_current_slot(jint) override;
	void set_motion_x(jdouble) override;
	void set_motion_y(jdouble) override;
	void set_motion_z(jdouble) override;
	void set_step_height(jfloat height) override;
	void set_pitch(jfloat) override;
	void set_yaw(jfloat) override;
	void set_velocity(jdouble, jdouble, jdouble) override;
	void set_position(jdouble, jdouble, jdouble) override;
	void set_angles(jfloat, jfloat) override;
	void set_sneaking(jboolean) override;
	void set_sprinting(jboolean) override;
};

class c_world_1710 : public c_world
{
public:
	void instantiate(jobject, JNIEnv*) override;
	std::vector<std::shared_ptr<c_player>> get_players() override;
	std::shared_ptr<c_block> get_block(jint, jint, jint) override;
};

class c_game_1710 : public c_game
{
public:
	void instantiate(JNIEnv*) override;

	jboolean in_game_has_focus() override;
	jboolean is_in_inventory() override;
	jboolean is_view_bobbing() override;
	jboolean is_in_chat() override;

	void set_view_bobbing(jboolean) override;
	void set_timer_speed(jfloat) override;
	void set_right_click_delay(jint) override;
	void set_in_focus() override;
	void set_not_in_focus() override;

	std::shared_ptr<c_player> get_player() override;
	std::shared_ptr<c_world> get_world() override;

	jfloat get_render_partial_ticks() override;
	jfloat get_mouse_sensitivity() override;
	jfloat get_timer_speed() override;

	jobject get_player_controller() override;

	jint get_right_click_delay() override;
	jint get_string_width(jstring) override;

	jint draw_string_with_shadow(jstring, jint, jint, jint) override;

	jobject get_net_handler() override;

	void enable_light_map() override;
	void set_pointed_entity(jobject ent) override;
	void set_object_mouse_over(jobject player) override;
	void disable_light_map() override;
	void setup_camera_transform(jfloat, int) override;
	void set_mouse_grabbed(jboolean grabbed) override;

	jlong get_window_handle() override;
	jint get_screen_w() override;
	jint get_screen_h() override;
	void set_cursor_pos(jint, jint) override;
};
