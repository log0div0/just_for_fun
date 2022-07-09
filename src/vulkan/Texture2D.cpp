#include "Texture2D.hpp"
#include "Context.hpp"
#include "details/PhysicalDevice.hpp"
#include <scope_guard.hpp>

namespace vulkan {

Texture2D::Texture2D(const fs::path& path) {
	format = vk::Format::eR8G8B8A8Srgb;

	stb::Image img(path, 4);
	Buffer staging_buffer(img.data, img.data_len());

	vk::ImageCreateInfo image_info{
		.sType = vk::StructureType::eImageCreateInfo,
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = {
			.width = (uint32_t)img.w,
			.height = (uint32_t)img.h,
			.depth = 1,
		},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = vk::ImageTiling::eOptimal,
		.usage = vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined,
	};

	image = vk::raii::Image(g_context->device, image_info);

	InitMemory();

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

	vk::raii::CommandBuffer command_buffer = g_context->BeginCommandBuffer();
	SCOPE_SUCCESS{ g_context->EndCommandBuffer(std::move(command_buffer)); };

	{
		vk::ImageMemoryBarrier barrier{
			.sType = vk::StructureType::eImageMemoryBarrier,
			.srcAccessMask = vk::AccessFlagBits::eNone,
			.dstAccessMask = vk::AccessFlagBits::eTransferWrite,
			.oldLayout = vk::ImageLayout::eUndefined,
			.newLayout = vk::ImageLayout::eTransferDstOptimal,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = *image,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};

		command_buffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTopOfPipe,
			vk::PipelineStageFlagBits::eTransfer,
			vk::DependencyFlags{},
			{}, {}, {barrier});
	}

	{
		vk::BufferImageCopy buffer_image_copy {
			.bufferOffset = 0,
			.bufferRowLength = 0,
			.bufferImageHeight = 0,
			.imageSubresource = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.mipLevel = 0,
				.baseArrayLayer = 0,
				.layerCount = 1,
			},
			.imageOffset = {0, 0, 0},
			.imageExtent = {
			    (uint32_t)img.w,
			    (uint32_t)img.h,
			    1
			},
		};

		command_buffer.copyBufferToImage(*staging_buffer.buffer, *image, vk::ImageLayout::eTransferDstOptimal, {buffer_image_copy});
	}

	{
		vk::ImageMemoryBarrier barrier{
			.sType = vk::StructureType::eImageMemoryBarrier,
			.srcAccessMask = vk::AccessFlagBits::eTransferWrite,
			.dstAccessMask = vk::AccessFlagBits::eShaderRead,
			.oldLayout = vk::ImageLayout::eTransferDstOptimal,
			.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = *image,
			.subresourceRange = {
				.aspectMask = vk::ImageAspectFlagBits::eColor,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1,
			}
		};

		command_buffer.pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eFragmentShader,
			vk::DependencyFlags{},
			{}, {}, {barrier});
	}
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

}
