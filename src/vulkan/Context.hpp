
#pragma once

#include "../rhi/Context.hpp"

#include "details/Frame.hpp"

#include <stack>

#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "Mesh.hpp"

#include "../Window.hpp"

namespace vulkan {

enum {
	SRV_TABLE_SIZE = 8,
	CBV_TABLE_SIZE = UNIFORM_BUFFERS_COUNT,
	SAMPLER_TABLE_SIZE = 4
};

struct Context: rhi::Context {
	Context(Window& window_);
	virtual ~Context() override;

	virtual rhi::Texture2D* CreateTexture2D(const fs::path& path) override { return new Texture2D(path); }
	virtual rhi::ShaderProgram* CreateShaderProgram(const fs::path& path) override { return new ShaderProgram(path); }
	virtual rhi::Mesh* CreateMesh(const fs::path& path) override { return new Mesh(path); }

	virtual void Clear() override;
	virtual void Present() override;
	virtual void WaitIdle() override;

	virtual void ImGuiInit() override;
	virtual void ImGuiShutdown() override;
	virtual void ImGuiNewFrame() override;
	virtual void ImGuiRender() override;

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
	uint32_t frame_count = 0;
	uint32_t queue_family_index = 0;

	void InitDevice();
	vk::raii::Device device = nullptr;

	void InitQueue();
	vk::raii::Queue queue = nullptr;

	void InitDescriptorPool();
	vk::raii::DescriptorPool descriptor_pool = nullptr;

	void InitDescriptorSetLayouts();
	vk::raii::DescriptorSetLayout cbv_set_layout = nullptr;
	vk::raii::DescriptorSetLayout srv_set_layout = nullptr;

	void InitPipelineLayout();
	vk::raii::PipelineLayout pipeline_layout = nullptr;

	void InitCommandPool();
	vk::raii::CommandPool command_pool = nullptr;

	void InitDepthStencilTexture(int w, int h);
	Texture2D depth_stencil_texture;

	void InitDefaultSampler();
	vk::raii::Sampler default_sampler = nullptr;

	void InitRenderPass();
	vk::raii::RenderPass render_pass = nullptr;

	void Resize(int w, int h);
	void InitSwapchain(int w, int h);
	vk::SwapchainCreateInfoKHR swapchain_info = {};
	vk::raii::SwapchainKHR swapchain = nullptr;

	void InitNextFrameSemaphore();
	vk::raii::Semaphore next_frame_semaphore = nullptr;

	void InitFrames();
	std::vector<Frame> frames;

	Frame* current_frame = nullptr;

	vk::raii::CommandBuffer BeginCommandBuffer();
	void EndCommandBuffer(vk::raii::CommandBuffer command_buffer);

	void SetSRV(size_t binding, Texture2D& texture);

	vk::raii::DescriptorSet CommitCBs();
	vk::raii::DescriptorSet CommitSRVs();
	void CommitAll();

	void InitSRVTable();
	std::array<vk::DescriptorImageInfo, SRV_TABLE_SIZE> srv_table = {};
};

extern Context* g_context;

}
