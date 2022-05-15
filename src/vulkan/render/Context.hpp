
#pragma once

#include <glfwpp/glfwpp.h>
#include <vulkan/vulkan_raii.hpp>

namespace render {

struct Context {
	Context(glfw::Window& window_);
	~Context();

	void Clear();
	void Present();
	void WaitIdle();

	glfw::Window& window;

	vk::raii::Context vk_context;

	void InitVkInstance();
	vk::raii::Instance vk_instance = nullptr;
};

extern Context* g_context;

}
