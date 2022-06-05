#include "Image.hpp"
#include "../Context.hpp"

namespace vulkan {

Image::Image(int index_): index(index_) {
	InitImageView();
	InitFramebuffer();
	InitCommandBuffer();
}

void Image::InitImageView() {
	vk::ImageViewCreateInfo image_view_info {
		.sType = vk::StructureType::eImageViewCreateInfo,
		.image = g_context->swapchain.getImages().at(index),
		.viewType = vk::ImageViewType::e2D,
		.format = g_context->swapchain_info.imageFormat,
		.components = {
			.r = vk::ComponentSwizzle::eIdentity,
			.g = vk::ComponentSwizzle::eIdentity,
			.b = vk::ComponentSwizzle::eIdentity,
			.a = vk::ComponentSwizzle::eIdentity,
		},
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1,
		},
	};

	image_view = vk::raii::ImageView(g_context->device, image_view_info);
}

void Image::InitFramebuffer() {
	std::vector<vk::ImageView> attachments = {*image_view};

	vk::FramebufferCreateInfo framebuffer_info{
		.sType = vk::StructureType::eFramebufferCreateInfo,
		.renderPass = *g_context->render_pass,
		.attachmentCount = (uint32_t)attachments.size(),
		.pAttachments = attachments.data(),
		.width = g_context->swapchain_info.imageExtent.width,
		.height = g_context->swapchain_info.imageExtent.height,
		.layers = 1,
	};

	framebuffer = vk::raii::Framebuffer(g_context->device, framebuffer_info);
}

void Image::InitCommandBuffer() {
	vk::CommandBufferAllocateInfo alloc_info{
		.sType = vk::StructureType::eCommandBufferAllocateInfo,
		.commandPool = *g_context->command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1,
	};
	vk::raii::CommandBuffers command_buffers(g_context->device, alloc_info);
	command_buffer = std::move(command_buffers[0]);
}

void Image::BeginFrame() {
	command_buffer.reset();

	vk::CommandBufferBeginInfo begin_info {
		.sType = vk::StructureType::eCommandBufferBeginInfo,
	};
	command_buffer.begin(begin_info);

	vk::ClearColorValue clear_color(std::array<float,4>{0.0f, 0.0f, 0.0f, 1.0f});
	std::vector<vk::ClearValue> clear_values = {clear_color};
	vk::RenderPassBeginInfo begin_pass_info{
		.sType = vk::StructureType::eRenderPassBeginInfo,
		.renderPass = *g_context->render_pass,
		.framebuffer = *framebuffer,
		.renderArea = {
			.offset = {0, 0},
			.extent = g_context->swapchain_info.imageExtent,
		},
		.clearValueCount = (uint32_t)clear_values.size(),
		.pClearValues = clear_values.data(),
	};
	command_buffer.beginRenderPass(begin_pass_info, vk::SubpassContents::eInline);
}

void Image::EndFrame() {
	command_buffer.endRenderPass();
	command_buffer.end();
}

}
