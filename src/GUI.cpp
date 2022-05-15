
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

static void CheckVkResult(VkResult err)
{
	if (err == 0) {
		return;
	}
	throw std::runtime_error(__PRETTY_FUNCTION__);
}

void GUI::ImplInit(glfw::Window& window) {
	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_InitInfo init_info = {
		.Instance = *render::g_context->vk_instance,
		.PhysicalDevice = *render::g_context->physical_device,
		.Device = *render::g_context->device,
		.QueueFamily = render::g_context->queue_family_index,
		.Queue = *render::g_context->queue,
		.PipelineCache = VK_NULL_HANDLE,
		.DescriptorPool = *render::g_context->descriptor_pool,
		.Subpass = 0,
		.MinImageCount = render::g_context->image_count,
		.ImageCount = render::g_context->image_count,
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
		.Allocator = nullptr,
		.CheckVkResultFn = CheckVkResult,
	};
	ImGui_ImplVulkan_Init(&init_info, *render::g_context->render_pass);

	// Upload Fonts
	{
		vk::CommandBufferAllocateInfo command_buffer_info{
			.sType = vk::StructureType::eCommandBufferAllocateInfo,
			.commandPool = *render::g_context->command_pool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1,
		};
		vk::raii::CommandBuffers command_buffers(render::g_context->device, command_buffer_info);
		vk::raii::CommandBuffer command_buffer = std::move(command_buffers[0]);

		vk::CommandBufferBeginInfo begin_info {
			.sType = vk::StructureType::eCommandBufferBeginInfo,
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};
		command_buffer.begin(begin_info);
		ImGui_ImplVulkan_CreateFontsTexture(*command_buffer);
		command_buffer.end();

		std::vector<vk::CommandBuffer> command_buffers_to_submit = {*command_buffer};
		vk::SubmitInfo submit_info = {
			.sType = vk::StructureType::eSubmitInfo,
			.commandBufferCount = (uint32_t)command_buffers_to_submit.size(),
			.pCommandBuffers = command_buffers_to_submit.data(),
		};
		render::g_context->queue.submit({submit_info}, VK_NULL_HANDLE);
		render::g_context->queue.waitIdle();

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

void GUI::ImplShutdown() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void GUI::ImplNewFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void GUI::ImplRender() {
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *render::g_context->current_image->command_buffer);
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
