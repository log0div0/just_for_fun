#include "Texture2D.hpp"
#include "Context.hpp"
#include "details/PhysicalDevice.hpp"
#include <scope_guard.hpp>

namespace vulkan {

void Texture2D::InitMemory() {
	vk::MemoryRequirements mem_requirements = image.getMemoryRequirements();

	vk::MemoryAllocateInfo alloc_info = {
		.sType = vk::StructureType::eMemoryAllocateInfo,
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = FindMemoryType(g_context->physical_device, mem_requirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal),
	};

	memory = vk::raii::DeviceMemory(g_context->device, alloc_info);

	image.bindMemory(*memory, 0);
}

}
