#include "context.h"
#include "menu.h"

UI::c_window loader;

void c_gui::draw()
{
	static UI::c_text_input username;
	static UI::c_text_input password;
	static UI::c_button login;
	static char username_str[32]{};
	static char password_str[32]{};

	loader.handle();

	loader.column_center();
	{
		username.handle(loader.data(), xors("username"), username_str);
		password.handle(loader.data(), xors("password"), password_str, true);

		login.handle(loader.data(), xors("login"), []() {
			return;
			});
	}
}