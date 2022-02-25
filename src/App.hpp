
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <glfwpp/glfwpp.h>

#include "BoxActor.hpp"
#include "Camera.hpp"

struct App {
	App(glfw::Window window_);
	~App();

	void Run();

private:
	void InitWindow();
	void InitRenderer();
	void InitInput();
	void InitWorld();

	void ProcessInput(float delta_time);
	void UpdateWorld(float delta_time);
	void Render();

	glfw::Window window;

	Camera camera;
	BoxActor box;

	using CursorPos = math::Vector<double, 2>;
	CursorPos last_cursor_pos;
};
