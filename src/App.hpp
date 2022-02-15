
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

	GLuint shader_program = 0;
	GLuint vertex_array = 0;
	GLuint vertex_pos_buffer = 0;
	GLuint index_buffer = 0;

	glfw::Window window;
};
