#pragma once

#include "Buffer.hpp"
#include "../../rhi/Context.hpp"

namespace vulkan {

struct Image {
	Image(int index_);

	int index = -1;
	vk::raii::ImageView image_view = nullptr;
	vk::raii::Framebuffer framebuffer = nullptr;
	vk::raii::CommandBuffer command_buffer = nullptr;
	std::array<Buffer, UNIFORM_BUFFERS_COUNT> uniform_buffers;
	vk::raii::DescriptorSet cbv_set = nullptr;
	vk::raii::DescriptorSet srv_set = nullptr;

	void InitImageView();
	void InitFramebuffer();
	void InitCommandBuffer();
	void InitUniformBuffers();
	void InitCBVSet();
	void InitSRVSet();

	void BeginFrame();
	void EndFrame();

	void CommitAll();
};

}
