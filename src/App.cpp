
#include "App.hpp"
#include "Utils.hpp"
#include <iostream>
#include <scope_guard.hpp>

App::App(fs::path assets_dir_): assets_dir(std::move(assets_dir_))
{
	InitWindow();
	InitShaders();
	InitMesh();
}

App::~App()
{
	if (shader_program) {
		glDeleteProgram(shader_program);
	}
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

void App::InitWindow() {
	int w = 640;
	int h = 480;

	window = glfw::Window{640, 480, "Hello World"};
	glfw::makeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	window.framebufferSizeEvent.setCallback([](glfw::Window& window, int w, int h) {
		glViewport(0, 0, w, h);
	});
	glViewport(0, 0, w, h);
}

void App::InitShaders() {
	shader_program = LoadShaderProgram(assets_dir / "shader.vert", assets_dir / "shader.frag");
}

void App::InitMesh() {
  float vertices[] = {
		-0.5f, -0.5f, 0.0f, // left
		0.5f, -0.5f, 0.0f, // right
		0.0f,  0.5f, 0.0f  // top
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	WithVertexArray(vao, [&]
	{
		WithBuffer(GL_ARRAY_BUFFER, vbo, [&]
		{
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		});
	});
}

void App::Run()
{
	while (!window.shouldClose())
	{
		ProcessInput();
		Render();

		window.swapBuffers();
		glfw::pollEvents();
	}
}

void App::ProcessInput()
{
	if (window.getKey(glfw::KeyCode::Escape)) {
		window.setShouldClose(true);
	}
}

void App::Render()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(shader_program);
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 3);
}
