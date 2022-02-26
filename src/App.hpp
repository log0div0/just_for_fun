
#pragma once

#include "GUI.hpp"
#include "BoxActor.hpp"
#include "PointLight.hpp"
#include "Camera.hpp"

struct App {
	App(glfw::Window& window_);
	~App();

	void Run();

private:
	void InitWindow();
	void InitRenderer();
	void InitWorld();

	void Update(float delta_time);
	void Render();

	glfw::Window& window;

	Camera camera;
	PointLight light;
	BoxActor box;
	GUI gui;
};
