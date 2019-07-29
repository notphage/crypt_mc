#include "context.h"
#include "input.h"
#include <string>

c_input_manager g_input;

void c_input_manager::capture_mouse_move(long lparam)
{
	m_mouse_pos[0] = LOWORD(lparam);
	m_mouse_pos[1] = HIWORD(lparam);
}

void c_input_manager::clear()
{
	A_memset(m_pressed_keys, 0, sizeof(m_pressed_keys));
	A_memset(m_mouse_pos, 0, sizeof(m_mouse_pos));

	m_scroll_wheel_state = 0;
	char m_keyboard_char = 0;
}

bool c_input_manager::register_key_press(VirtualKeyEvents_t key_event, VirtualKeys_t key)
{
	switch (key_event)
	{
	case KEYDOWN:
	{
		if (is_valid_key(key))
			m_pressed_keys[key] = true;
		return true;
	}
	case KEYUP:
	{
		if (is_valid_key(key))
			m_pressed_keys[key] = false;
		return true;
	}
	case SYSKEYDOWN:
	{ //WTF IS THIS STUPID SHIT, WHY IS ALT LITERALLY THE ONLY FUCKING KEY UNDER SYSKEYDOWN OUT OF ALL THE MODIFIER KEYS?
		if (key == KEYS_ALT)
			m_pressed_keys[key] = true;
		break;
	}
	case SYSKEYUP:
	{
		if (key == KEYS_ALT)
			m_pressed_keys[key] = false;
		break;
	}
	case LBUTTONDOWN:
		m_pressed_keys[KEYS_MOUSE1] = true;
		return true;
	case LBUTTONUP:
		m_pressed_keys[KEYS_MOUSE1] = false;
		return true;
	case RBUTTONDOWN:
		m_pressed_keys[KEYS_MOUSE2] = true;
		return true;
	case RBUTTONUP:
		m_pressed_keys[KEYS_MOUSE2] = false;
		return true;
	case MBUTTONDOWN:
		m_pressed_keys[KEYS_MOUSE3] = true;
		return true;
	case MBUTTONUP:
		m_pressed_keys[KEYS_MOUSE3] = false;
		return true;
	case XBUTTONDOWN:
	{
		bool pressed_xbutton = static_cast<bool>(HIWORD(key) - 1); //should result in mouse4 as false, and mouse5 as true
		m_pressed_keys[pressed_xbutton ? KEYS_MOUSE5 : KEYS_MOUSE4] = true;
		return true;
	}
	case XBUTTONUP:
	{
		bool pressed_xbutton = static_cast<bool>(HIWORD(key) - 1); //should result in mouse4 as false, and mouse5 as true
		m_pressed_keys[pressed_xbutton ? KEYS_MOUSE5 : KEYS_MOUSE4] = false;
		return true;
	}
	case MOUSEWHEEL:
	{
		short scroll_input = (short)HIWORD(key);
		m_scroll_wheel_state = scroll_input > 0 ? 1 : -1;
		return true;
	}
	case CHAR_INPUT:
	{
		if (key == 0x8 || (key >= 0x20 && key <= 0x7E))
			m_keyboard_char = (char)(key);
	}
	}

	return key_event == 0x200 || key_event == 0x203 || key_event == 0x206 || key_event == 0x209; //gotta block WM_MOUSEFIST | WM_LBUTTONDBLCLK | WM_RBUTTONDBLCLK | WM_MBUTTONDBLCLK
}

bool c_input_manager::is_key_pressed(int key)
{
	auto k = static_cast<VirtualKeys_t>(key);
	return is_valid_key(k) && m_pressed_keys[k];
}

bool c_input_manager::is_key_pressed(keysetting_t key)
{
	VirtualKeys_t pressed = (VirtualKeys_t)key.key;
	keytype_t keytype = (keytype_t)key.type;
	bool valid{};
	static bool toggled[ARRAYSIZE(m_pressed_keys)];
	static bool pressed_once[ARRAYSIZE(toggled)];

	if (keytype == kt_always)
		return true;

	if (keytype == kt_hold)
		return is_valid_key(pressed) && m_pressed_keys[pressed];

	if (keytype == kt_toggle)
	{
		if (is_key_pressed(pressed))
			pressed_once[pressed] = true;

		if (!is_key_pressed(pressed) && pressed_once[pressed])
		{
			toggled[pressed] = !toggled[pressed];
			pressed_once[pressed] = false;
		}

		return toggled[pressed];
	}

	return false;
}

std::string c_input_manager::get_key_name(VirtualKeys_t key)
{
	std::vector<std::string> key_names = [&]()
	{
		std::vector<std::string> yeet = {
			xors("unknown"),
			xors("mouse1"),
			xors("mouse2"),
			xors("cancel"),
			xors("mouse3"),
			xors("mouse4"),
			xors("mouse5"),
			xors("unknown"),
			xors("backspace"),
			xors("tab"),
			xors("unknown"),
			xors("unknown"),
			xors("clear"),
			xors("return"),
			xors("unknown"),
			xors("unknown"),
			xors("shift"),
			xors("control"),
			xors("alt"),
			xors("pause"),
			xors("caps lock"),
			xors("kana"),
			xors("unknown"),
			xors("junja"),
			xors("final"),
			xors("kanji"),
			xors("unknown"),
			xors("escape"),
			xors("convert"),
			xors("nonconvert"),
			xors("accept"),
			xors("modechange"),
			xors("space"),
			xors("prior"),
			xors("next"),
			xors("end"),
			xors("home"),
			xors("left"),
			xors("up"),
			xors("right"),
			xors("down"),
			xors("select"),
			xors("print"),
			xors("execute"),
			xors("snapshot"),
			xors("ins"),
			xors("delete"),
			xors("help"),
			xors("0"),
			xors("1"),
			xors("2"),
			xors("3"),
			xors("4"),
			xors("5"),
			xors("6"),
			xors("7"),
			xors("8"),
			xors("9"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("a"),
			xors("b"),
			xors("c"),
			xors("d"),
			xors("e"),
			xors("f"),
			xors("g"),
			xors("h"),
			xors("i"),
			xors("j"),
			xors("k"),
			xors("l"),
			xors("m"),
			xors("n"),
			xors("o"),
			xors("p"),
			xors("q"),
			xors("r"),
			xors("s"),
			xors("t"),
			xors("u"),
			xors("v"),
			xors("w"),
			xors("x"),
			xors("y"),
			xors("z"),
			xors("lwin"),
			xors("rwin"),
			xors("apps"),
			xors("unknown"),
			xors("unknown"),
			xors("numpad0"),
			xors("numpad1"),
			xors("numpad2"),
			xors("numpad3"),
			xors("numpad4"),
			xors("numpad5"),
			xors("numpad6"),
			xors("numpad7"),
			xors("numpad8"),
			xors("numpad9"),
			xors("multiply"),
			xors("add"),
			xors("separator"),
			xors("subtract"),
			xors("decimal"),
			xors("divide"),
			xors("f1"),
			xors("f2"),
			xors("f3"),
			xors("f4"),
			xors("f5"),
			xors("f6"),
			xors("f7"),
			xors("f8"),
			xors("f9"),
			xors("f10"),
			xors("f11"),
			xors("f12"),
			xors("f13"),
			xors("f14"),
			xors("f15"),
			xors("f16"),
			xors("f17"),
			xors("f18"),
			xors("f19"),
			xors("f20"),
			xors("f21"),
			xors("f22"),
			xors("f23"),
			xors("f24"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("numlock"),
			xors("scroll"),
			xors("oem_nec_equal"),
			xors("oem_fj_masshou"),
			xors("oem_fj_touroku"),
			xors("oem_fj_loya"),
			xors("oem_fj_roya"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("unknown"),
			xors("lshift"),
			xors("rshift"),
			xors("lcontrol"),
			xors("rcontrol"),
			xors("lmenu"),
			xors("rmenu"),
		};
		return yeet;
	}();

	//	for ( size_t i = 0; i < key_names.size( ); ++i )
	//	printf( "%s\n", key_names[i].c_str( ) );

	if (!is_valid_key(key))
		return key_names[KEYS_NONE];

	return key_names[(int)key];
}

VirtualKeys_t c_input_manager::is_any_key_pressed()
{
	for (size_t i{}; i < KEYS_MAX; ++i)
	{
		if (m_pressed_keys[i])
		{
			return VirtualKeys_t(i);
		}
	}

	return KEYS_NONE;
}

bool c_input_manager::is_any_input_active()
{
	if (is_any_key_pressed() > KEYS_NONE)
		return true;

	if (get_scroll_state() != 0)
		return true;

	return false;
}

int c_input_manager::get_scroll_state()
{
	int current_state = m_scroll_wheel_state;
	m_scroll_wheel_state = 0;
	return current_state;
}

char c_input_manager::get_pressed_char(VirtualKeys_t* out)
{
	unsigned long pressed_character{};
	for (unsigned long i{}; i < KEYS_MAX; ++i)
	{
		if (is_key_pressed(VirtualKeys_t(i)))
		{
			if ((i >= KEYS_A && i <= KEYS_Z)
				|| (i >= KEYS_N0 && i <= KEYS_N9))
			{
				pressed_character = i;
			}
		}
	}

	if (pressed_character)
	{
		if (out)
		{
			*out = VirtualKeys_t(pressed_character);
		}

		if (is_key_pressed(KEYS_SHIFT))
		{
			if (pressed_character >= KEYS_A
				&& pressed_character <= KEYS_Z)
				return char(pressed_character);

			//gay way to shift it to symbols
			if (pressed_character >= KEYS_N0
				&& pressed_character <= KEYS_N9)
			{
				switch (pressed_character)
				{
				case KEYS_N0:
					return ')';
				case KEYS_N1:
					return '!';
				case KEYS_N2:
					return '@';
				case KEYS_N3:
					return '#';
				case KEYS_N4:
					return '$';
				case KEYS_N5:
					return '%';
				case KEYS_N6:
					return '^';
				case KEYS_N7:
					return '&';
				case KEYS_N8:
					return '*';
				case KEYS_N9:
					return '(';
				}
			}
		}
		else
		{
			if (pressed_character >= KEYS_A
				&& pressed_character <= KEYS_Z)
				return char(tolower(pressed_character));

			if (pressed_character >= KEYS_N0
				&& pressed_character <= KEYS_N9)
				return char(pressed_character);
		}
	}
	else if (is_key_pressed(KEYS_SPACE))
	{
		if (out)
			* out = KEYS_SPACE;

		return ' ';
	}
	else if (is_key_pressed(KEYS_BACK))
	{
		if (out)
			* out = KEYS_BACK;

		return 0;
	}

	if (out)
		* out = KEYS_NONE;

	return 0;
}

void c_input_manager::press_mouse1()
{
	int x, y;
	get_cursor_pos(x, y);

	LI_FN(SendMessageA).cached()(ctx.m_window, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y));
}

void c_input_manager::release_mouse1()
{
	int x, y;
	get_cursor_pos(x, y);

	LI_FN(SendMessageA).cached()(ctx.m_window, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(x, y));
}
