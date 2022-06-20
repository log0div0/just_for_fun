#include "Frame.hpp"
#include "../Context.hpp"

namespace vulkan {

Frame::Frame(int index_): index(index_) {
	InitImageView();
	InitFramebuffer();
	InitCommandBuffer();
	InitUniformBuffers();
	InitCBVSet();
	InitSRVSet();
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

void Frame::InitUniformBuffers() {
	for (size_t i = 0; i < UNIFORM_BUFFERS_COUNT; ++i)
	{
		uniform_buffers[i] = Buffer(UNIFORM_BUFFER_SIZE,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	}
}

void Frame::InitCBVSet() {
	std::vector<vk::DescriptorSetLayout> layouts { *g_context->cbv_set_layout };
	vk::DescriptorSetAllocateInfo alloc_info{
		.sType = vk::StructureType::eDescriptorSetAllocateInfo,
		.descriptorPool = *g_context->descriptor_pool,
		.descriptorSetCount = (uint32_t)layouts.size(),
		.pSetLayouts = layouts.data(),
	};

	cbv_set = std::move(vk::raii::DescriptorSets(g_context->device, alloc_info)[0]);

	std::array<vk::DescriptorBufferInfo, CBV_TABLE_SIZE> buffer_info = {};
	std::array<vk::WriteDescriptorSet, CBV_TABLE_SIZE> writes = {};

	for (size_t i = 0; i < CBV_TABLE_SIZE; ++i)
	{
		buffer_info[i] = vk::DescriptorBufferInfo{
			.buffer = *uniform_buffers[i],
			.offset = 0,
			.range = uniform_buffers[i].info.size,
		};

		writes[i] = vk::WriteDescriptorSet{
			.sType = vk::StructureType::eWriteDescriptorSet,
			.dstSet = *cbv_set,
			.dstBinding = (uint32_t)i,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eUniformBuffer,
			.pBufferInfo = &buffer_info[i],
		};
	}

	g_context->device.updateDescriptorSets(writes, {});
}

void Frame::InitSRVSet() {
	std::vector<vk::DescriptorSetLayout> layouts { *g_context->srv_set_layout };
	vk::DescriptorSetAllocateInfo alloc_info{
		.sType = vk::StructureType::eDescriptorSetAllocateInfo,
		.descriptorPool = *g_context->descriptor_pool,
		.descriptorSetCount = (uint32_t)layouts.size(),
		.pSetLayouts = layouts.data(),
	};

	srv_set = std::move(vk::raii::DescriptorSets(g_context->device, alloc_info)[0]);

	std::array<vk::DescriptorImageInfo, SRV_TABLE_SIZE> image_info = {};
	std::array<vk::WriteDescriptorSet, SRV_TABLE_SIZE> writes = {};

	for (size_t i = 0; i < SRV_TABLE_SIZE; ++i)
	{
		image_info[i] = vk::DescriptorImageInfo{
			.sampler = *g_context->null_texture->sampler,
			.imageView = *g_context->null_texture->image_view,
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal,
		};

		writes[i] = vk::WriteDescriptorSet{
			.sType = vk::StructureType::eWriteDescriptorSet,
			.dstSet = *srv_set,
			.dstBinding = (uint32_t)i,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = vk::DescriptorType::eCombinedImageSampler,
			.pImageInfo = &image_info[i],
		};
	}

	g_context->device.updateDescriptorSets(writes, {});
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

void Frame::EndFrame() {
	command_buffer.endRenderPass();
	command_buffer.end();
}

void Frame::CommitAll() {
	for (size_t i = 0; i < UNIFORM_BUFFERS_COUNT; ++i) {
		auto& src = g_context->uniform_buffers[i];
		if (!src.GetSize()) {
			continue;
		}
		auto& dst = uniform_buffers[i];
		void* p = dst.memory.mapMemory(0, src.GetSize());
		memcpy(p, src.GetData(), src.GetSize());
		dst.memory.unmapMemory();
		src.Reset();
	}
	command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *g_context->pipeline_layout, 0, {*cbv_set, *srv_set}, {});
}

}
