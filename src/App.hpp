
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <glfwpp/glfwpp.h>
#include <mogl/mogl.hpp>

struct App {
	App(glfw::Window window_, fs::path assets_dir_);
	~App();

	void Run();

private:
	void InitWindow();
	void InitShaders();
	void InitTextures();
	void InitMesh();

	void ProcessInput();
	void Render();

	glfw::Window window;
	fs::path assets_dir;

	mogl::ShaderProgram shader_program;
	mogl::Texture wood_texture = GL_TEXTURE_2D;
	mogl::Texture lambda_texture = GL_TEXTURE_2D;
	mogl::ArrayBuffer vertex_buffer;
	mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;
};
