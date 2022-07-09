#pragma once

#include <stb/Image.hpp>

#include "../Utils.hpp"

#include "../rhi/Texture2D.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace vulkan {

struct Texture2D: rhi::Texture2D {
	Texture2D() = default;
	Texture2D(const fs::path& path)
	{
	}

	vk::Format format = vk::Format::eUndefined;

	vk::raii::Image image = nullptr;

	void InitMemory();
	vk::raii::DeviceMemory memory = nullptr;

	vk::raii::ImageView image_view = nullptr;
};

}
