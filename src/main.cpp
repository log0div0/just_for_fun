
#include <iostream>
#include <glad/glad.h>
#include <glfwpp/glfwpp.h>

void ProcessInput(glfw::Window& window) {
	if (window.getKey(glfw::KeyCode::Escape)) {
		window.setShouldClose(true);
	}
}

int main() {
	auto GLFW = glfw::init();

	glfw::WindowHints{  .contextVersionMajor = 4,
						.contextVersionMinor = 6,
						.openglProfile = glfw::OpenGlProfile::Core }.apply();

	int w = 640;
	int h = 480;

	glfw::Window window{640, 480, "Hello World"};
	glfw::makeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	window.framebufferSizeEvent.setCallback([](glfw::Window& window, int w, int h) {
		glViewport(0, 0, w, h);
	});
	glViewport(0, 0, w, h);

	while (!window.shouldClose())
	{
		ProcessInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		window.swapBuffers();
		glfw::pollEvents();
	}

	return 0;
}
