#include "context.h"
#include "menu.h"

UI::c_window loader;
UI::c_console loader_console;
c_client client;

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
			client.set_stage(connection_stage::STAGE_LOGIN);
			loader.m_current_stage = PAGE_LANDING;
			});
	}
}

void c_gui::draw_landing()
{
	static UI::c_dropdown cheat_dropdown;
	static UI::c_button load;

	std::lock_guard<std::mutex> lock(ctx.m_game_list_mutex);

	std::vector<std::string> cheats;
	for (auto&& cheat : ctx.m_game_list)
		cheats.push_back(cheat.m_name);

	static int old_cheat = ctx.m_selected_cheat;

	loader.column_center();
	{ 
		cheat_dropdown.handle(loader.data(), xors("cheats"), cheats, &ctx.m_selected_cheat, 5);

		loader_console.handle(loader.data());

		load.handle(loader.data(), xors("load"), []()
			{
				client.set_stage(connection_stage::STAGE_SELECT);
			});

		cheat_dropdown.dropdown(loader.data());
	}

	if (ctx.m_selected_cheat != old_cheat && !ctx.m_game_list.empty())
	{
		const auto& cheat = ctx.m_game_list.at(ctx.m_selected_cheat);
		loader_console.insert(cheat.m_name + std::string(": ") + client.game_status_to_string(cheat.m_status) + std::string(" (") + std::to_string(cheat.m_version) + ")", color_t(255, 255, 255));
		old_cheat = ctx.m_selected_cheat;
	}
}

void c_gui::draw()
{
	loader.handle();

	switch (loader.m_current_stage)
	{
		case PAGE_LOGIN:
		{
			draw_login();
			break;
		}

		case PAGE_LANDING:
		{
			draw_landing();
			break;
		}
	}
}

void c_gui::insert_text(const std::string& str, const color_t& col)
{
	loader_console.insert(str, col);
}
