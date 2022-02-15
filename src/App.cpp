
#include "App.hpp"
#include "Utils.hpp"
#include <scope_guard.hpp>

App::App(fs::path assets_dir_): assets_dir(std::move(assets_dir_))
{
	InitWindow();
	InitShaders();
}

App::~App()
{

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
}
