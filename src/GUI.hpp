
#pragma once

#include "Utils.hpp"
#include "PointLight.hpp"

struct GUI {
	GUI();
	~GUI();

	GUI(const GUI& other) = delete;
	GUI(GUI&& other) = delete;

	GUI& operator=(const GUI& other) = delete;
	GUI& operator=(GUI&& other) = delete;

	void Update(float delta_time, PointLight& light);
	void Render();

	bool limit_framerate = true;
};
