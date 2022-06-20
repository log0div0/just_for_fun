#include "Texture2D.hpp"
#include "Context.hpp"
#include "details/PhysicalDevice.hpp"
#include <scope_guard.hpp>

namespace vulkan {

Texture2D::Texture2D(size_t w, size_t h) {
	InitImage(w, h);
	InitMemory();
	InitImageView();
	InitSampler();

	vk::raii::CommandBuffer command_buffer = g_context->BeginCommandBuffer();
	SCOPE_SUCCESS{ g_context->EndCommandBuffer(std::move(command_buffer)); };

	Barrier(command_buffer, vk::ImageLayout::eUndefined, vk::ImageLayout::eShaderReadOnlyOptimal);
}

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

void Texture2D::InitSampler() {
	vk::PhysicalDeviceProperties properties = g_context->physical_device.getProperties();

	vk::SamplerCreateInfo sampler_info{
		.sType = vk::StructureType::eSamplerCreateInfo,
		.magFilter = vk::Filter::eLinear,
		.minFilter = vk::Filter::eLinear,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eRepeat,
		.addressModeV = vk::SamplerAddressMode::eRepeat,
		.addressModeW = vk::SamplerAddressMode::eRepeat,
		.anisotropyEnable = VK_FALSE,
		.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
		.compareEnable = VK_FALSE,
		.compareOp = vk::CompareOp::eAlways,
		.borderColor = vk::BorderColor::eIntOpaqueBlack,
		.unnormalizedCoordinates = VK_FALSE,
	};

	sampler = vk::raii::Sampler(g_context->device, sampler_info);
}

vk::AccessFlagBits GetAccess(vk::ImageLayout layout) {
	switch (layout) {
		case vk::ImageLayout::eUndefined:
			return vk::AccessFlagBits::eNone;
		case vk::ImageLayout::eTransferDstOptimal:
			return vk::AccessFlagBits::eTransferWrite;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			return vk::AccessFlagBits::eShaderRead;
		default:
			throw std::runtime_error(__PRETTY_FUNCTION__);
	}
}

vk::PipelineStageFlags GetPipelineStage(vk::ImageLayout layout) {
	switch (layout) {
		case vk::ImageLayout::eUndefined:
			return vk::PipelineStageFlagBits::eTopOfPipe;
		case vk::ImageLayout::eTransferDstOptimal:
			return vk::PipelineStageFlagBits::eTransfer;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			return vk::PipelineStageFlagBits::eFragmentShader;
		default:
			throw std::runtime_error(__PRETTY_FUNCTION__);
	}
}

void Texture2D::Barrier(vk::raii::CommandBuffer& command_buffer, vk::ImageLayout old_layout, vk::ImageLayout new_layout) {
	vk::ImageMemoryBarrier barrier{
		.sType = vk::StructureType::eImageMemoryBarrier,
		.srcAccessMask = GetAccess(old_layout),
		.dstAccessMask = GetAccess(new_layout),
		.oldLayout = old_layout,
		.newLayout = new_layout,
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
		GetPipelineStage(old_layout),
		GetPipelineStage(new_layout),
		vk::DependencyFlags{},
		{}, {}, {barrier});
}

}
