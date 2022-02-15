
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
	void InitMesh();

	void ProcessInput();
	void Render();

	fs::path assets_dir;

	uint32_t shader_program = 0;
	uint32_t vbo = 0, vao = 0;

	glfw::Window window;
};
