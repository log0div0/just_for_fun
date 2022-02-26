
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <glfwpp/glfwpp.h>

#include "GUI.hpp"
#include "BoxActor.hpp"
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
	BoxActor box;
	GUI gui;
};
