
#include "App.hpp"
#include "Utils.hpp"
#include <math/vector.hpp>
#include <iostream>
#include <scope_guard.hpp>
#include <stb/Image.hpp>

using namespace math::literals;

App::App(glfw::Window window_, fs::path assets_dir_): window(std::move(window_)), assets_dir(std::move(assets_dir_))
{
	InitWindow();
	InitShaders();
	InitTextures();
	InitMesh();
	InitRenderer();
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

	vertex_shader.compile(LoadTextFile(assets_dir / "shaders" / "shader.vert"));
	fragment_shader.compile(LoadTextFile(assets_dir / "shaders" / "shader.frag"));

	shader_program.attach(vertex_shader);
	shader_program.attach(fragment_shader);
	shader_program.link();

	shader_program.setUniform("Wood", 3);
	shader_program.setUniform("Lambda", 1);
}

void App::InitTextures() {
	{
		stb::Image img(assets_dir / "textures" / "Wood_Crate_001_basecolor.jpg", 3);
		wood_texture.setStorage2D(5, GL_RGBA8, img.w, img.h);
		wood_texture.setSubImage2D(0, 0, 0, img.w, img.h, GL_RGB, GL_UNSIGNED_BYTE, img.data);
		wood_texture.generateMipmap();
	}

	{
		stb::Image img(assets_dir / "textures" / "Half-Life_lambda_logo.png", 4);
		lambda_texture.setStorage2D(5, GL_RGBA8, img.w, img.h);
		lambda_texture.setSubImage2D(0, 0, 0, img.w, img.h, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
		lambda_texture.generateMipmap();
	}
}

struct Vertex {
	math::Vector3 pos;
	// math::Vector3 color;
	math::Vector2 uv;
};

std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f, -0.5f},  {0.0f, 0.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f} }
};

std::vector<uint32_t> indices = {
	0, 1, 3,
	1, 2, 3
};

void App::InitMesh() {

	GLuint pos = shader_program.getAttribLocation("aPos");
	// GLuint color = shader_program.getAttribLocation("aColor");
	GLuint uv = shader_program.getAttribLocation("aUV");
	GLuint binding_index = 1; // any vacant value

	vertex_buffer.setData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

	vertex_array.setAttribBinding(pos, binding_index);
	vertex_array.setAttribFormat(pos, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	vertex_array.enableAttrib(pos);

	// vertex_array.setAttribBinding(color, binding_index);
	// vertex_array.setAttribFormat(color, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	// vertex_array.enableAttrib(color);

	vertex_array.setAttribBinding(uv, binding_index);
	vertex_array.setAttribFormat(uv, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	vertex_array.enableAttrib(uv);

	index_buffer.setData(indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	vertex_array.setElementBuffer(index_buffer.getHandle());
}

void App::InitRenderer() {
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glClearDepth(0.0);
}

void App::Run()
{
	while (!window.shouldClose())
	{
		double start = glfwGetTime();

		ProcessInput();
		UpdateWorld();
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

void App::UpdateWorld()
{
	auto [w, h] = window.getFramebufferSize();
	float aspect = float(w) / h;
	model = math::Rotate(GetTimeSeconds() * 0.1f, {0.0f, 0.0f, 1.0f});
	view = math::LookAt({2.0f, 1.0f, 2.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f});
	projection = math::Perspective(60_deg, aspect, 0.1f, 1000.0f);
	projection.m.At(1,1) *= -1;
	shader_program.setUniformMatrixPtr<4, float>("model", (float*)&model);
	shader_program.setUniformMatrixPtr<4, float>("view", (float*)&view);
	shader_program.setUniformMatrixPtr<4, float>("projection", (float*)&projection);
}

void App::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader_program.use();
	vertex_array.bind();
	wood_texture.bind(3);
	lambda_texture.bind(1);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
