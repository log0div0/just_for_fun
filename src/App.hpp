
#pragma once

#include "GUI.hpp"
#include "BoxActor.hpp"
#include "PointLight.hpp"
#include "Camera.hpp"
#include "rhi/Context.hpp"

struct App {
	App(Window& window_);
	~App();

	void Run();

private:
	void Update(float delta_time);
	void Render();

	Window& window;

	Camera camera;
	PointLight light;
	BoxActor box;
	GUI gui;
};
