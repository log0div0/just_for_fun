#include "Texture2D.hpp"
#include "Context.hpp"
#include "details/PhysicalDevice.hpp"
#include <scope_guard.hpp>

namespace vulkan {

void Texture2D::InitImage(size_t w, size_t h) {
	vk::ImageCreateInfo image_info{
		.sType = vk::StructureType::eImageCreateInfo,
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = {
			.width = (uint32_t)w,
			.height = (uint32_t)h,
			.depth = 1,
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = vk::ImageUsageFlagBits::eSampled,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,

	};

	image = vk::raii::Image(g_context->device, image_info);
}

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

void Texture2D::InitImageView() {
	vk::ImageViewCreateInfo view_info{
		.sType = vk::StructureType::eImageViewCreateInfo,
		.image = *image,
		.viewType = vk::ImageViewType::e2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	image_view = vk::raii::ImageView(g_context->device, view_info);
}

}
