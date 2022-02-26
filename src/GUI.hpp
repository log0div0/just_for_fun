
#pragma once

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <glfwpp/glfwpp.h>

struct GUI {
	GUI(glfw::Window& window);
	~GUI();

	GUI(const GUI& other) = delete;
	GUI(GUI&& other) = delete;

	GUI& operator=(const GUI& other) = delete;
	GUI& operator=(GUI&& other) = delete;

	void Update(float delta_time);
	void Render();
};
