
#pragma once

#include "Utils.hpp"

struct GUI {
	GUI(glfw::Window& window);
	~GUI();

	GUI(const GUI& other) = delete;
	GUI(GUI&& other) = delete;

	GUI& operator=(const GUI& other) = delete;
	GUI& operator=(GUI&& other) = delete;

	void Update(float delta_time);
	void Render();

	bool limit_framerate = true;
};
