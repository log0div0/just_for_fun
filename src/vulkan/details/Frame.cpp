#include "Frame.hpp"
#include "../Context.hpp"

namespace vulkan {

Frame::Frame(int index_): index(index_) {
	InitImageView();
	InitFramebuffer();
	InitCommandBuffer();
	InitSemaphores();
	InitFence();
}

void Frame::InitImageView() {
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

void Frame::InitFramebuffer() {
	std::vector<vk::ImageView> attachments = {*image_view, *g_context->depth_stencil_texture.image_view};

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

void Frame::InitCommandBuffer() {
	vk::CommandBufferAllocateInfo alloc_info{
		.sType = vk::StructureType::eCommandBufferAllocateInfo,
		.commandPool = *g_context->command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1,
	};
	vk::raii::CommandBuffers command_buffers(g_context->device, alloc_info);
	command_buffer = std::move(command_buffers[0]);
}

void Frame::InitSemaphores() {
	vk::SemaphoreCreateInfo semaphore_info = {
		.sType = vk::StructureType::eSemaphoreCreateInfo,
	};
	image_available_semaphore = vk::raii::Semaphore(g_context->device, semaphore_info);
	render_finished_semaphore = vk::raii::Semaphore(g_context->device, semaphore_info);
}

void Frame::InitFence() {
	vk::FenceCreateInfo fence_info = {
		.sType = vk::StructureType::eFenceCreateInfo,
	};
	fence = vk::raii::Fence(g_context->device, fence_info);
}

void Frame::BeginFrame() {
	command_buffer.reset();

	vk::CommandBufferBeginInfo begin_info {
		.sType = vk::StructureType::eCommandBufferBeginInfo,
	};
	command_buffer.begin(begin_info);

	vk::ClearColorValue clear_color(std::array<float,4>{0.2f, 0.3f, 0.3f, 1.0f});
	vk::ClearDepthStencilValue clear_depth_stencil(0.0f, 0);
	std::vector<vk::ClearValue> clear_values = {clear_color, clear_depth_stencil};
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

void Frame::EndFrame() {
	command_buffer.endRenderPass();
	command_buffer.end();
}

void Frame::Submit() {
	std::vector<vk::Semaphore> wait_semaphores = {*image_available_semaphore};
	std::vector<vk::PipelineStageFlags> wait_stages = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
	std::vector<vk::CommandBuffer> command_buffers = {*command_buffer};
	std::vector<vk::Semaphore> signal_semaphores = {*render_finished_semaphore};

	vk::SubmitInfo submit_info{
		.sType = vk::StructureType::eSubmitInfo,
		.waitSemaphoreCount = (uint32_t)wait_semaphores.size(),
		.pWaitSemaphores = wait_semaphores.data(),
		.pWaitDstStageMask = wait_stages.data(),
		.commandBufferCount = (uint32_t)command_buffers.size(),
		.pCommandBuffers = command_buffers.data(),
		.signalSemaphoreCount = (uint32_t)signal_semaphores.size(),
		.pSignalSemaphores = signal_semaphores.data(),
	};

	g_context->queue.submit({submit_info}, *fence);
}

void Frame::Present() {
	std::vector<vk::Semaphore> wait_semaphores = {*render_finished_semaphore};
	std::vector<uint32_t> image_indexes = {(uint32_t)index};
	std::vector<vk::SwapchainKHR> swapchains = {*g_context->swapchain};

	vk::PresentInfoKHR present_info {
		.sType = vk::StructureType::ePresentInfoKHR,
		.waitSemaphoreCount = (uint32_t)wait_semaphores.size(),
		.pWaitSemaphores = wait_semaphores.data(),
		.swapchainCount = (uint32_t)swapchains.size(),
		.pSwapchains = swapchains.data(),
		.pImageIndices = image_indexes.data(),
	};

	vk::Result result = g_context->queue.presentKHR(present_info);
	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("queue.presentKHR() failed");
	}
}

void Frame::Wait() {
	vk::Result result = g_context->device.waitForFences(*fence, true, UINT64_MAX);
	if (result != vk::Result::eSuccess) {
		throw std::runtime_error("device.waitForFences() failed");
	}
	g_context->device.resetFences(*fence);
}

void Frame::Reset() {
	descriptor_set_refs.clear();
	uniform_buffer_refs.clear();
}

}
