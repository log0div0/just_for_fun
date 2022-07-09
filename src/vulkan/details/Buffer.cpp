#include "Buffer.hpp"
#include "PhysicalDevice.hpp"
#include "../Context.hpp"
#include <scope_guard.hpp>

namespace vulkan {

Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
	info = {
		.sType = vk::StructureType::eBufferCreateInfo,
		.size = size,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
	};

	buffer = vk::raii::Buffer(g_context->device, info);

	vk::MemoryRequirements memory_requirements = buffer.getMemoryRequirements();

	uint32_t memory_type_index = FindMemoryType(g_context->physical_device, memory_requirements.memoryTypeBits, properties);

	vk::MemoryAllocateInfo alloc_info{
		.sType = vk::StructureType::eMemoryAllocateInfo,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = memory_type_index,
	};

	memory = vk::raii::DeviceMemory(g_context->device, alloc_info);

	buffer.bindMemory(*memory, 0);
}

Buffer::Buffer(uint8_t* data, size_t size):
	Buffer(size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent)
{
	uint8_t* p = (uint8_t*)memory.mapMemory(0, size);
	memcpy(p, data, size);
	memory.unmapMemory();
}

void Buffer::copy_to(Buffer& other) {
	if (this->info.size > other.info.size) {
		throw std::runtime_error("src size > dst size");
	}

	vk::raii::CommandBuffer command_buffer = g_context->BeginCommandBuffer();
	SCOPE_SUCCESS{ g_context->EndCommandBuffer(std::move(command_buffer)); };

	vk::BufferCopy copy_region{
		.size = info.size
	};
	command_buffer.copyBuffer(*this->buffer, *other.buffer, {copy_region});
}

}
