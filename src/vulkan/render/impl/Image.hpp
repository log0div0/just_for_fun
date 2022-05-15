#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace render {

struct Image {
	Image(int index_);

	int index = -1;
	vk::raii::ImageView image_view = nullptr;
	vk::raii::Framebuffer framebuffer = nullptr;
	vk::raii::CommandBuffer command_buffer = nullptr;

	void InitImageView();
	void InitFramebuffer();
	void InitCommandBuffer();

	void BeginFrame();
	void EndFrame();
};

}
