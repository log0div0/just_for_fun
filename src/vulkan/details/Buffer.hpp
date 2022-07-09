
#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace vulkan {

struct Buffer {
	Buffer() = default;
	Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	Buffer(const uint8_t* data, size_t size, vk::BufferUsageFlags usage);

	void copy_to(Buffer& other);

	vk::Buffer operator*() const {
		return *buffer;
	}

	vk::BufferCreateInfo info = {};
	vk::raii::Buffer buffer = nullptr;
	vk::raii::DeviceMemory memory = nullptr;
};

}
