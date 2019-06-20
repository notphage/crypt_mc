#pragma once

struct string_setting_t
{
	char text[128];
};
class node_t
{
public:
	virtual ~node_t() = default;

	virtual void add(node_t* node_ptr) = 0;

	virtual void load(const char* path, const char* file) = 0;
	virtual void save(const char* path, const char* file) const = 0;
};

class holder_t : public node_t
{
public:
	__forceinline holder_t(const char* name) : m_name(name) { }

	__forceinline holder_t(holder_t* holder_ptr, const char* name) : m_name(name) { holder_ptr->add(this); }

	void add(node_t* node_ptr) override
	{
		m_nodes.push_back(node_ptr);
	}

	void load(const char* path, const char* file) override
	{
		char full_path[64];
		strcpy_s(full_path, path);
		strcat_s(full_path, "_");
		strcat_s(full_path, m_name);
		for (auto x : m_nodes)
			x->load(full_path, file);
	}

	void save(const char* path, const char* file) const override
	{
		char full_path[64];
		strcpy_s(full_path, path);
		strcat_s(full_path, "_");
		strcat_s(full_path, m_name);
		for (auto x : m_nodes)
			x->save(full_path, file);
	}

	const char* m_name;
	std::vector< node_t* > m_nodes;
};

class ISetting : public node_t
{
	void add(node_t* node) override { }
};

template < typename T >
class setting_t : ISetting
{
public:
	__forceinline setting_t(holder_t* holder_ptr, uint32_t name) : m_name(name) { holder_ptr->add(this); }

	__forceinline setting_t(holder_t* holder_ptr, uint32_t name, const T& rhs) : m_value(rhs), m_default(rhs), m_name(name) { holder_ptr->add(this); }

	void load(const char* path, const char* file) override
	{
		m_value = m_default;
		util::simple_load(m_name, &m_value, sizeof(m_value), file);
	}

	void save(const char* path, const char* file) const override
	{
		util::simple_save(m_name, &m_value, sizeof(m_value), file);
	}

	__forceinline operator T& ()
	{
		return m_value;
	}

	__forceinline operator const T& () const
	{
		return m_value;
	}

	__forceinline T* operator &()
	{
		return &m_value;
	}

	__forceinline const T* operator &() const
	{
		return &m_value;
	}

	__forceinline T& operator ()()
	{
		return m_value;
	}

	__forceinline const T& operator ()() const
	{
		return m_value;
	}

private:
	T m_value;
	T m_default;
	uint32_t m_name;
};

// yes....
//extern uint32_t get_iter();

// aight y'all
template< typename t >
class settingtype_t
{
public:
	settingtype_t(const t& value = {})
	{
		for (size_t i = 0; i < 1; ++i)
			m_data[i] = value;
	}

	t m_data[1];

	// GOD I'M SO FUCKING SMART, WORKS WITH DEFAULT VALUE IN SETTING_T CTOR
	// IF OYU ACTUALLY WANT TO SET THE VALUE, USE ( set_awp ) = ETC 
	/*void operator=( const t& value )
	{
		for ( size_t i = 0; i < set_max; ++i )
			m_data[i] = value;
	}*/

	t& operator( )()
	{
		return m_data[0];
	}

	t& operator[](const size_t& iter)
	{
		return m_data[iter];
	}
};

// fuck it!
struct settingstring_t
{
	char buffer[128];

	settingstring_t()
	{
		clear();
	}

	settingstring_t(const char* text)
	{
		set(text);
	}

	__forceinline void set(std::string& str)
	{
		set(str.c_str());
	}

	__forceinline void set(const char* text)
	{
		clear();
		strncpy_s(buffer, 128, text, 128);
	}

	__forceinline std::string get() const
	{
		return buffer;
	}

	__forceinline bool empty() const
	{
		return buffer[0] == 0;
	}

	__forceinline void clear()
	{
		memset(buffer, 0, 128);
	}

	__forceinline const char* c_str()
	{
		return buffer;
	}
};

class c_settings
{
public:
	bool tooltips = false;
	holder_t m_holder{ "" };

	//Combat features
	setting_t<bool> combat_auto_clicker{&m_holder, fnvc("combat_auto_clicker"), false};
	setting_t<keysetting_t> combat_auto_clicker_key{&m_holder, fnvc("combat_auto_clicker_key"), {}};
	setting_t<int> combat_auto_clicker_min_cps{&m_holder, fnvc("combat_auto_clicker_min_cps"), 11};
	setting_t<int> combat_auto_clicker_max_cps{&m_holder, fnvc("combat_auto_clicker_max_cps"), 14};

	setting_t<bool> combat_auto_clicker_weapons_only{&m_holder, fnvc("combat_auto_clicker_weapons_only"), true};
	setting_t<bool> combat_auto_clicker_inventory{&m_holder, fnvc("combat_auto_clicker_inventory"), true};
	setting_t<bool> combat_auto_clicker_block_hit{&m_holder, fnvc("combat_auto_clicker_block_hit"), false};

	setting_t<bool> combat_aim_assist{&m_holder, fnvc("combat_aim_assist"), false};
	setting_t<keysetting_t> combat_aim_assist_key{&m_holder, fnvc("combat_aim_assist_key"), {}};

	setting_t<int> combat_aim_assist_fov{&m_holder, fnvc("combat_aim_assist_fov"), 60};
	setting_t<float> combat_aim_assist_h_speed{&m_holder, fnvc("combat_aim_assist_h_speed"), 2.f};
	setting_t<float> combat_aim_assist_v_speed{&m_holder, fnvc("combat_aim_assist_v_speed"), 1.f};

	setting_t<float> combat_aim_assist_scale{&m_holder, fnvc("combat_aim_assist_scale"), 0.4f};
	setting_t<float> combat_aim_assist_distance{&m_holder, fnvc("combat_aim_assist_distance"), 4.0f};

	setting_t<bool> combat_aim_assist_sticky{ &m_holder, fnvc("combat_aim_assist_sticky"), true };
	setting_t<bool> combat_aim_assist_vertical{ &m_holder, fnvc("combat_aim_assist_vertical"), false };
	setting_t<bool> combat_aim_assist_require_click{&m_holder, fnvc("combat_aim_assist_require_click"), true};
	setting_t<bool> combat_aim_assist_weapons_only{&m_holder, fnvc("combat_aim_assist_weapons_only"), true};
	setting_t<bool> combat_aim_assist_invisibles{&m_holder, fnvc("combat_aim_assist_invisibles"), false};

	setting_t<bool> combat_velocity{&m_holder, fnvc("combat_velocity"), false};
	setting_t<keysetting_t> combat_velocity_key{&m_holder, fnvc("combat_velocity_key"), {}};
	setting_t<bool> combat_velocity_on_sprint{ &m_holder, fnvc("combat_velocity_on_sprint"), false };
	setting_t<int> combat_velocity_delay{ &m_holder, fnvc("combat_velocity_delay"), 0 };
	setting_t<int> combat_velocity_chance{ &m_holder, fnvc("combat_velocity_chance"), 80 };
	setting_t<int> combat_velocity_vertical{&m_holder, fnvc("combat_velocity_vertical"), 100};
	setting_t<int> combat_velocity_horizontal{&m_holder, fnvc("combat_velocity_horizontal"), 85};

	setting_t<bool> combat_throwpot{&m_holder, fnvc("combat_throwpot"), false};
    setting_t<keysetting_t> combat_throwpot_key{ &m_holder, fnvc("combat_throwpot_key"), {} };
	setting_t<int> combat_throwpot_delay{ &m_holder, fnvc("combat_throwpot_delay"), 50 };
	setting_t<int> combat_throwpot_health{ &m_holder, fnvc("combat_throwpot_health"), 12 };
    setting_t<bool> combat_throwpot_potions{ &m_holder, fnvc("combat_throwpot_potions"), true };
    setting_t<bool> combat_throwpot_soup{ &m_holder, fnvc("combat_throwpot_soup"), true };

	setting_t<bool> combat_reach{&m_holder, fnvc("combat_reach"), false};
	setting_t<keysetting_t> combat_reach_key{&m_holder, fnvc("combat_reach_key"), {}};
	setting_t<bool> combat_reach_on_sprint{ &m_holder, fnvc("combat_reach_on_sprint"), false };
	setting_t<float> combat_reach_distance_min{ &m_holder, fnvc("combat_reach_distance_min"), 3.f };
	setting_t<float> combat_reach_distance_max{ &m_holder, fnvc("combat_reach_distance_max"), 3.2f };

	//Player features
	setting_t<bool> player_fast_place{&m_holder, fnvc("player_fast_place"), false};
	setting_t<keysetting_t> player_fast_place_key{&m_holder, fnvc("player_fast_place_key"), {}};
	setting_t<int> player_fast_place_delay{&m_holder, fnvc("player_fast_place_delay"), 0};

	//Movement features
	setting_t<bool> movement_timer{&m_holder, fnvc("movement_timer"), false};
	setting_t<keysetting_t> movement_timer_key{&m_holder, fnvc("movement_timer_key"), {}};
	setting_t<float> movement_timer_speed{&m_holder, fnvc("movement_timer_speed"), 1.f};

    setting_t<bool> movement_air_control{ &m_holder, fnvc("movement_air_control"), false };
    setting_t<keysetting_t> movement_air_control_key{ &m_holder, fnvc("movement_air_control_key"), {} };

    setting_t<bool> movement_step{ &m_holder, fnvc("movement_step"), false };
    setting_t<keysetting_t> movement_step_key{ &m_holder, fnvc("movement_step_key"), {} };
    setting_t<float> movement_step_height{ &m_holder, fnvc("movement_step_height"), 1.0f };

    setting_t<bool> movement_flight{ &m_holder, fnvc("movement_flight"), false };
    setting_t<keysetting_t> movement_flight_key{ &m_holder, fnvc("movement_flight_key"), {} };
    setting_t<bool> movement_flight_tight{ &m_holder, fnvc("movement_flight_tight"), false };
    setting_t<bool> movement_flight_glide{ &m_holder, fnvc("movement_flight_glide"), false };
    setting_t<float> movement_flight_hspeed{ &m_holder, fnvc("movement_flight_hspeed"), 0.4f };
    setting_t<float> movement_flight_vspeed{ &m_holder, fnvc("movement_flight_vspeed"), 0.4f };
    setting_t<float> movement_flight_glide_speed{ &m_holder, fnvc("movement_flight_glide_speed"), 0.08f };

    setting_t<bool> movement_sprint{ &m_holder, fnvc("movement_sprint"), false };
    setting_t<keysetting_t> movement_sprint_key{ &m_holder, fnvc("movement_sprint_key"), {} };

	setting_t<bool> movement_speed{&m_holder, fnvc("movement_speed"), false};
	setting_t<keysetting_t> movement_speed_key{&m_holder, fnvc("movement_speed_key"), {}};
    setting_t<int> movement_speed_mode{ &m_holder, fnvc("movement_speed_mode"), 0 };

	setting_t<int> visuals_esp_box{&m_holder, fnvc("visuals_esp_box"), 0};
	setting_t<color_t> visuals_esp_box_color{&m_holder, fnvc("visuals_esp_box_color"), {255, 200, 69, 255}};
	setting_t<bool> visuals_esp_box_filled{&m_holder, fnvc("visuals_esp_box_filled"), false};
	setting_t<color_t> visuals_esp_box_outline_color{&m_holder, fnvc("visuals_esp_box_outline_color"), {255, 200, 69, 255}};
	setting_t<bool> visuals_esp_snap_lines{ &m_holder, fnvc("visuals_esp_snap_lines"), false };
	setting_t<color_t> visuals_esp_snap_lines_color{ &m_holder, fnvc("visuals_esp_snap_lines_color"), {255, 200, 69, 255} };
	setting_t<bool> visuals_esp_nametags{ &m_holder, fnvc("visuals_esp_nametags"), false };

	// config
	setting_t<keysetting_t> menu_key{ &m_holder, fnvc("menu_key"), {KEYS_INSERT, kt_toggle} };
	setting_t<keysetting_t> hide_key{ &m_holder, fnvc("hide_key"), {} };
	setting_t<color_t> gui_accent_color{ &m_holder, fnvc("gui_accent_color"), { 181, 0, 41, 255 } };
	setting_t<int> gui_fade_speed{ &m_holder, fnvc("menu_fade_speed"), 8 };
	setting_t<bool> gui_rainbow{ &m_holder, fnvc("gui_rainbow"), false };
	setting_t<bool> log_use_accent_color{ &m_holder, fnvc("log_use_accent_color"), false };
	setting_t<color_t> log_color{ &m_holder, fnvc("log_color"), { 181, 0, 41, 255 } };
	setting_t<bool> watermark{ &m_holder, fnvc("watermark"), true };
	setting_t<bool> show_keybinds{ &m_holder, fnvc("show_keybinds"), false };
	setting_t<uint64_t> log{ &m_holder, fnvc("log"), 0 };

	// Keybinds
	setting_t<int> gui_keybinds_list_pos_x{ &m_holder, fnvc("gui_keybinds_list_pos_x"), 1000 };
	setting_t<int> gui_keybinds_list_pos_y{ &m_holder, fnvc("gui_keybinds_list_pos_y"), 200 };


	setting_t<vec2> gui_menu_pos{ &m_holder, fnvc("gui_menu_pos"), vec2(100, 100) };

	void save(const char* name) { m_holder.save("", name); }
	void load(const char* name) { m_holder.load("", name); }
};
