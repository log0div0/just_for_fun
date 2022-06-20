#pragma once

#include "Buffer.hpp"
#include "../../rhi/Context.hpp"

namespace vulkan {

struct Frame {
	Frame(int index_);

	int index = -1;

	void InitImageView();
	vk::raii::ImageView image_view = nullptr;

	void InitFramebuffer();
	vk::raii::Framebuffer framebuffer = nullptr;

	void InitCommandBuffer();
	vk::raii::CommandBuffer command_buffer = nullptr;

	void InitUniformBuffers();
	std::array<Buffer, UNIFORM_BUFFERS_COUNT> uniform_buffers;

	void InitCBVSet();
	vk::raii::DescriptorSet cbv_set = nullptr;

	void InitSRVSet();
	vk::raii::DescriptorSet srv_set = nullptr;

	void InitSemaphores();
	vk::raii::Semaphore image_available_semaphore = nullptr;
	vk::raii::Semaphore render_finished_semaphore = nullptr;

	void InitFence();
	vk::raii::Fence fence = nullptr;

	void BeginFrame();
	void EndFrame();

	void CommitAll();
};

}
