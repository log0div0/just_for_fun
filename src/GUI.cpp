
#include "GUI.hpp"
#include "rhi/Context.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"

GUI::GUI() {
	ImGui::CreateContext();
	rhi::g_context->ImGuiInit();
}

GUI::~GUI() {
	rhi::g_context->ImGuiShutdown();
	ImGui::DestroyContext();
}

void GUI::Update(float delta_time, PointLight& light) {
	rhi::g_context->ImGuiNewFrame();
	ImGui::NewFrame();

	{
		ImGui::Begin("Hello, world!");

		ImGui::ColorEdit3("Light color", (float*)&light.color);

		ImGui::Checkbox("Limit frame rate", &limit_framerate);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

void GUI::Render() {
	ImGui::Render();
	rhi::g_context->ImGuiRender();
}
