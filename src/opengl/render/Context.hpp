
#pragma once

#include <glad/glad.h>
#include <mogl/mogl.hpp>

namespace render {

struct Context {
	Context(glfw::Window& window_): window(window_) {
		glfw::makeContextCurrent(window);
		glfw::swapInterval(0);
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			throw std::runtime_error("Failed to initialize GLAD");
		}

		window.framebufferSizeEvent.subscribe([&](glfw::Window& window, int w, int h) {
			glViewport(0, 0, w, h);
		});
		auto [w, h] = window.getFramebufferSize();
		glViewport(0, 0, w, h);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_GREATER);
		glClearDepth(0.0);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

	}

	void Clear() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Present() {
		window.swapBuffers();
	}

	void WaitIdle() {

	}

	glfw::Window& window;
};

}
