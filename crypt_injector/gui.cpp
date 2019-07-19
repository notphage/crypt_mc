#include "context.h"
#include "menu.h"

UI::c_window loader;

void c_gui::draw_login()
{
	static UI::c_text_input username;
	static UI::c_text_input password;
	static UI::c_button login;
	static char username_str[32]{};
	static char password_str[32]{};

	loader.column_center();
	{
		username.handle(loader.data(), xors("username"), username_str);
		password.handle(loader.data(), xors("password"), password_str, true);

		login.handle(loader.data(), xors("login"), []() {
			loader.m_current_stage = STAGE_LANDING;
			});
	}
}

void c_gui::draw_landing()
{
	static UI::c_console loader_console;
	static UI::c_dropdown cheat_dropdown;
	static UI::c_button load;

	std::vector<std::string> cheats =
	{
		"CS:GO",
		"Minecraft",
		"Minecraft Lite",
		"Minecraft Clicker",
		"Minecraft Blatant"
	};

	static int selected_cheat = -1;

	loader.column_center();
	{
		cheat_dropdown.handle(loader.data(), xors("cheats"), cheats, &selected_cheat, 5);

		loader_console.handle(loader.data());

		load.handle(loader.data(), xors("load"), []()
			{

			});

		cheat_dropdown.dropdown(loader.data());
	}
}

void c_gui::draw()
{
	loader.handle();

	switch (loader.m_current_stage)
	{
		case STAGE_LOGIN:
		{
			draw_login();
			break;
		}

		case STAGE_LANDING:
		{
			draw_landing();
			break;
		}
	}
}