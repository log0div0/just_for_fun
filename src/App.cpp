
#include "App.hpp"
#include "Utils.hpp"
#include <math/vector.hpp>
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

struct Vertex {
	math::Vector3 pos;
	math::Vector3 color;
};

void App::InitMesh() {
	Vertex vertices[] = {
	    { { 0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f} },
	    { {-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f} },
	    { { 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f} }
	};
	unsigned int indices[] = {
		0, 1, 2,
	};

	GLuint vertex_pos = shader_program.getAttribLocation("vertex_pos");
	GLuint vertex_color = shader_program.getAttribLocation("vertex_color");
	GLuint binding_index = 1; // any vacant value

	vertex_pos_buffer.setData(sizeof(vertices), vertices, GL_STATIC_DRAW);
	index_buffer.setData(sizeof(indices), indices, GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_pos_buffer.getHandle(), 0, sizeof(Vertex));
	vertex_array.setAttribBinding(vertex_pos, binding_index);
	vertex_array.setAttribFormat(vertex_pos, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	vertex_array.enableAttrib(vertex_pos);
	vertex_array.setAttribBinding(vertex_color, binding_index);
	vertex_array.setAttribFormat(vertex_color, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	vertex_array.enableAttrib(vertex_color);
	vertex_array.setElementBuffer(index_buffer.getHandle());
}

void App::Run()
{
	while (!window.shouldClose())
	{
		double start = glfwGetTime();

		ProcessInput();
		Render();

		window.swapBuffers();
		glfw::pollEvents();

		while (true) {
			double finish = glfwGetTime();

			double frame_time = finish - start;
			const int target_fps = 30;
			double target_frame_time = 1.0 / target_fps;

			if (frame_time > target_frame_time) {
				break;
			}
			glfw::waitEvents(target_frame_time - frame_time);
		}
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

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
