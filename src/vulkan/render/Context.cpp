#include "Context.hpp"

namespace render {

Context* g_context = nullptr;

void Context::InitVkInstance() {
	const std::vector<const char*> layers = {
#ifndef NDEBUG
		"VK_LAYER_KHRONOS_validation",
#endif
	};
	const std::vector<const char*> extensions = glfw::getRequiredInstanceExtensions();

	vk::InstanceCreateInfo instance_info {
		.sType = vk::StructureType::eInstanceCreateInfo,
		.enabledLayerCount = (uint32_t)layers.size(),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = (uint32_t)extensions.size(),
		.ppEnabledExtensionNames = extensions.data(),
	};

	vk_instance = vk::raii::Instance(vk_context, instance_info);
}

Context::Context(glfw::Window& window_): window(window_) {
	g_context = this;
	InitVkInstance();
	throw std::runtime_error(__FUNCSIG__);
}

Context::~Context() {
	g_context = nullptr;
}

void Context::Clear() {
	throw std::runtime_error(__FUNCSIG__);
}

void Context::Present() {
	throw std::runtime_error(__FUNCSIG__);
}

void Context::WaitIdle() {
	throw std::runtime_error(__FUNCSIG__);
}

}
