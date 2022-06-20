#pragma once

#include <stb/Image.hpp>

#include "../Utils.hpp"

#include "../rhi/Texture2D.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace vulkan {

struct Texture2D: rhi::Texture2D {
	Texture2D(size_t w, size_t h);
	Texture2D(const fs::path& path)
	{
	}

	static const vk::Format format = vk::Format::eR8G8B8A8Srgb;

	void InitImage(size_t w, size_t h);
	vk::raii::Image image = nullptr;

	void InitMemory();
	vk::raii::DeviceMemory memory = nullptr;

	void InitImageView();
	vk::raii::ImageView image_view = nullptr;

	void InitSampler();
	vk::raii::Sampler sampler = nullptr;

	void Barrier(vk::raii::CommandBuffer& command_buffer, vk::ImageLayout old_layout, vk::ImageLayout new_layout);
};

}
