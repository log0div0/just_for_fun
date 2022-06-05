
#pragma once

#include "BoxMesh.hpp"
#include "ShaderProgram.hpp"
#include "Texture2D.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

namespace opengl {

struct Context: rhi::Context {
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

	virtual ~Context() override {}

	virtual rhi::Texture2D* CreateTexture2D(const fs::path& path) override { return new Texture2D(path); }
	virtual rhi::ShaderProgram* CreateShaderProgram(const std::string& name) override { return new ShaderProgram(name); }
	virtual rhi::BoxMesh* CreateBoxMesh() override { return new BoxMesh(); }

	virtual void Clear() override {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	virtual void Present() override {
		window.swapBuffers();
	}

	virtual void WaitIdle() override {

	}

	virtual void ImGuiInit() override {
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460 core");
	}
	virtual void ImGuiShutdown() override {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}
	virtual void ImGuiNewFrame() override {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}
	virtual void ImGuiRender() override {
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	glfw::Window& window;
};

}
