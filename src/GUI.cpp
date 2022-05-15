
#include "GUI.hpp"
#include "render/Context.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#ifdef OPENGL
#include "imgui_impl_opengl3.h"
#elif DX12
#include "imgui_impl_dx12.h"
#elif VULKAN
#include "imgui_impl_vulkan.h"
#endif

#ifdef OPENGL
void GUI::ImplInit(glfw::Window& window) {
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");
}

void GUI::ImplShutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void GUI::ImplNewFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void GUI::ImplRender() {
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#elif DX12
void GUI::ImplInit(glfw::Window& window) {
	auto srv_handle = render::g_context->view_heap.alloc();
	ImGui_ImplGlfw_InitForOther(window, true);
	ImGui_ImplDX12_Init(
		render::g_context->device,
		render::NUM_FRAMES_IN_FLIGHT,
        render::SwapChain::FORMAT,
        render::g_context->view_heap.heap,
        srv_handle.cpu,
        srv_handle.gpu
	);
}

void GUI::ImplShutdown() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void GUI::ImplNewFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void GUI::ImplRender() {
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), render::g_context->command_list);
}
#elif VULKAN
void GUI::ImplInit(glfw::Window& window) {
	ImGui_ImplGlfw_InitForOther(window, true);
	throw std::runtime_error(__FUNCSIG__); // ImGui_ImplVulkan_Init();
}

void GUI::ImplShutdown() {
	throw std::runtime_error(__FUNCSIG__); // ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void GUI::ImplNewFrame() {
	throw std::runtime_error(__FUNCSIG__); // ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void GUI::ImplRender() {
	throw std::runtime_error(__FUNCSIG__); // ImGui_ImplVulkan_RenderDrawData();
}
#endif

GUI::GUI(glfw::Window& window) {
	ImGui::CreateContext();
	ImplInit(window);
}

GUI::~GUI() {
	ImplShutdown();
	ImGui::DestroyContext();
}

void GUI::Update(float delta_time, PointLight& light) {
	ImplNewFrame();
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
	ImplRender();
}
