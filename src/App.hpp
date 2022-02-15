
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <glfwpp/glfwpp.h>

struct App {
	App(fs::path assets_dir_);
	~App();

	void Run();

private:
	void InitWindow();
	void InitShaders();

	void ProcessInput();
	void Render();

	fs::path assets_dir;

	glfw::Window window;
};
