
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
	if (vertex_array) {
		glDeleteVertexArrays(1, &vertex_array);
	}
	if (vertex_pos_buffer) {
		glDeleteBuffers(1, &vertex_pos_buffer);
	}
	if (index_buffer) {
		glDeleteBuffers(1, &index_buffer);
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
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
	};
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	glGenBuffers(1, &vertex_pos_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_pos_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
	glBindVertexArray(vertex_array);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
