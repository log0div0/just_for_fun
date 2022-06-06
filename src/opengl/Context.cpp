#include "Context.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace opengl {

Context::Context(glfw::Window& window_): window(window_) {
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

	for (size_t i = 0; i < UNIFORM_BUFFERS_COUNT; ++i) {
		mogl::UniformBuffer ub;
		ub.setStorage(UNIFORM_BUFFER_SIZE, nullptr, GL_DYNAMIC_STORAGE_BIT);
		gl_uniform_buffers.push_back(std::move(ub));
	}
}

void Context::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Context::Present() {
	window.swapBuffers();
}

void Context::WaitIdle() {

}

void Context::ImGuiInit() {
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}
void Context::ImGuiShutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}
void Context::ImGuiNewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}
void Context::ImGuiRender() {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Context::CommitResources() {
	for (size_t i = 0; i < uniform_buffers.size(); ++i) {
		rhi::UniformBuffer& ub = uniform_buffers[i];
		if (ub.GetSize()) {
			gl_uniform_buffers.at(i).setSubData(0, ub.GetSize(), ub.GetData());
			gl_uniform_buffers.at(i).bindBufferBase(i);
		}
		ub.Reset();
	}
}

}
