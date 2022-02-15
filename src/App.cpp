
#include "App.hpp"
#include "Utils.hpp"
#include <iostream>
#include <scope_guard.hpp>

App::App(glfw::Window window_, fs::path assets_dir_): window(std::move(window_)), assets_dir(std::move(assets_dir_))
{
	InitWindow();
	InitShaders();
	InitMesh();
}

App::~App()
{
}

void App::InitWindow() {
	int w = 640;
	int h = 480;

	window.setSize(w, h);

	window.framebufferSizeEvent.setCallback([](glfw::Window& window, int w, int h) {
		glViewport(0, 0, w, h);
	});
	glViewport(0, 0, w, h);

}

void App::InitShaders() {
	mogl::Shader vertex_shader(GL_VERTEX_SHADER);
	mogl::Shader fragment_shader(GL_FRAGMENT_SHADER);

	vertex_shader.compile(LoadTextFile(assets_dir / "shader.vert"));
	fragment_shader.compile(LoadTextFile(assets_dir / "shader.frag"));

	shader_program.attach(vertex_shader);
	shader_program.attach(fragment_shader);
	shader_program.link();
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

	vertex_pos_buffer.setData(sizeof(vertices), vertices, GL_STATIC_DRAW);
	index_buffer.setData(sizeof(indices), indices, GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(1, vertex_pos_buffer.getHandle(), 0, 3*sizeof(float));
	vertex_array.setAttribBinding(0, 1);
	vertex_array.setAttribFormat(0, 3, GL_FLOAT);
	vertex_array.enableAttrib(0);
	vertex_array.setElementBuffer(index_buffer.getHandle());
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

	shader_program.use();
	vertex_array.bind();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
