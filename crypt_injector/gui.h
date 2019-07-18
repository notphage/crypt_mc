#pragma once

#include <vector>
#include <string>
#include <iterator>
#include "renderer.h"

class c_gui
{
	enum stages_t
	{
		STAGE_LOGIN,
		STAGE_LANDING,
		STAGE_LOAD
	};

	void draw_login();
	void draw_landing();
	void draw_load();

public:
	void draw();
};