
#pragma once

#include "../rhi/Context.hpp"

#include "details/Frame.hpp"
#include "details/Image.hpp"

#include <stack>

#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "BoxMesh.hpp"

#include "../Window.hpp"

namespace vulkan {

struct Context: rhi::Context {
	Context(Window& window_);
	virtual ~Context() override;

	virtual rhi::Texture2D* CreateTexture2D(const fs::path& path) override { return new Texture2D(path); }
	virtual rhi::ShaderProgram* CreateShaderProgram(const std::string& name) override { return new ShaderProgram(name); }
	virtual rhi::BoxMesh* CreateBoxMesh() override { return new BoxMesh(); }

	virtual void Clear() override;
	virtual void Present() override;
	virtual void WaitIdle() override;

	virtual void ImGuiInit() override;
	virtual void ImGuiShutdown() override;
	virtual void ImGuiNewFrame() override;
	virtual void ImGuiRender() override;

	void CommitAll();

	Window& window;

	vk::raii::Context vk_context;

	void InitVkInstance();
	vk::raii::Instance vk_instance = nullptr;

	void InitSurface();
	vk::raii::SurfaceKHR surface = nullptr;

	void InitPhysicalDevice();
	vk::raii::PhysicalDevice physical_device = nullptr;

	vk::SurfaceFormatKHR surface_format = {};
	vk::PresentModeKHR present_mode = {};
	uint32_t image_count = 0;
	uint32_t queue_family_index = 0;

	void InitDevice();
	vk::raii::Device device = nullptr;

	void InitQueue();
	vk::raii::Queue queue = nullptr;

	void InitDescriptorSetLayout();
	vk::raii::DescriptorSetLayout descriptor_set_layout = nullptr;

	void InitDescriptorPool();
	vk::raii::DescriptorPool descriptor_pool = nullptr;

	void InitPipelineLayout();
	vk::raii::PipelineLayout pipeline_layout = nullptr;

	void InitCommandPool();
	vk::raii::CommandPool command_pool = nullptr;

	void InitRenderPass();
	vk::raii::RenderPass render_pass = nullptr;

	void Resize(int w, int h);
	void InitSwapchain(int w, int h);
	vk::SwapchainCreateInfoKHR swapchain_info = {};
	vk::raii::SwapchainKHR swapchain = nullptr;

	void InitImages();
	std::vector<Image> images;

	void InitFrames();
	std::vector<Frame> frames;
	std::stack<Frame*> free_frames;
	std::vector<Frame*> image_to_frame_map;

	Image* current_image = nullptr;
	Frame* current_frame = nullptr;
};

extern Context* g_context;

}
