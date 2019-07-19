#pragma once

#include <vector>
#include <string>
#include <iterator>
#include "renderer.h"

enum stages_t
{
	STAGE_LOGIN,
	STAGE_LANDING
};

class c_gui
{
	void draw_login();
	void draw_landing();

public:
	void draw();
};