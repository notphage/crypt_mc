#pragma once

#include <vector>
#include <string>
#include <iterator>
#include "renderer.h"

class c_gui
{
	enum page_t
	{
		PAGE_LOGIN,
		PAGE_LANDING
	};

	void draw_login();
	void draw_landing();

public:
	void draw();

	void insert_text(const std::string& str, const color_t& col);
};