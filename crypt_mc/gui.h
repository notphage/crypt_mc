#pragma once

#include <vector>
#include <string>
#include <iterator>
#include "renderer.h"

class c_gui
{
	enum tabs_t
	{
		TABS_COMBAT,
		TABS_MOVEMENT,
		TABS_PLAYER,
		TABS_VISUALS,
		TABS_MISC
	};

	std::vector<std::string> m_tabs =
	{
		//Later we may add exploits, but for now.

		xors("combat"),
		xors("movement"),
		xors("player"),
		xors("visuals"),
		xors("misc")
	};

	void tab_combat();
	void tab_movement();
	void tab_player();
	void tab_visuals();
	void tab_misc();

public:

	void draw();
	void background();
};

extern c_gui gui;