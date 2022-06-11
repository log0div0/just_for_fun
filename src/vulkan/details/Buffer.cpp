#include "Buffer.hpp"
#include "../Context.hpp"

namespace vulkan {

uint32_t FindMemoryType(uint32_t type_bits, vk::MemoryPropertyFlags property_flags) {
	vk::PhysicalDeviceMemoryProperties properties = g_context->physical_device.getMemoryProperties();
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
		if ((type_bits & (1 << i)) && (properties.memoryTypes[i].propertyFlags & property_flags) == property_flags) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
	info = {
		.sType = vk::StructureType::eBufferCreateInfo,
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
	};

	buffer = vk::raii::Buffer(g_context->device, info);

	vk::MemoryRequirements memory_requirements = buffer.getMemoryRequirements();

	uint32_t memory_type_index = FindMemoryType(memory_requirements.memoryTypeBits, properties);

	vk::MemoryAllocateInfo alloc_info{
		.sType = vk::StructureType::eMemoryAllocateInfo,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = memory_type_index,
	};

	memory = vk::raii::DeviceMemory(g_context->device, alloc_info);

	buffer.bindMemory(*memory, 0);
}

void Buffer::copy_to(Buffer& other) {
	if (this->info.size > other.info.size) {
		throw std::runtime_error("src size > dst size");
	}

	vk::CommandBufferAllocateInfo alloc_info{
		.sType = vk::StructureType::eCommandBufferAllocateInfo,
		.commandPool = *g_context->command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1,
	};

	vk::raii::CommandBuffer command_buffer = std::move(vk::raii::CommandBuffers(g_context->device, alloc_info)[0]);
	vk::CommandBufferBeginInfo begin_info{
		.sType = vk::StructureType::eCommandBufferBeginInfo,
		.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	};
	command_buffer.begin(begin_info);
	vk::BufferCopy copy_region{
		.size = info.size
	};
	command_buffer.copyBuffer(*this->buffer, *other.buffer, {copy_region});
	command_buffer.end();

	std::vector<vk::CommandBuffer> command_buffers = {*command_buffer};
	vk::SubmitInfo submit_info{
		.sType = vk::StructureType::eSubmitInfo,
		.commandBufferCount = (uint32_t)command_buffers.size(),
		.pCommandBuffers = command_buffers.data(),
	};

	g_context->queue.submit({submit_info}, nullptr);
	g_context->queue.waitIdle();
}

}
