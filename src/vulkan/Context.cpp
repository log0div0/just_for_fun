#include "Context.hpp"
#include "details/PhysicalDevice.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

namespace vulkan {

Context* g_context = nullptr;

static void CheckVkResult(VkResult err)
{
	if (err == 0) {
		return;
	}
	throw std::runtime_error(__PRETTY_FUNCTION__);
}

void Context::ImGuiInit() {
	ImGui_ImplGlfw_InitForVulkan(window.GetGLFW(), true);
	ImGui_ImplVulkan_InitInfo init_info = {
		.Instance = *vk_instance,
		.PhysicalDevice = *physical_device,
		.Device = *device,
		.QueueFamily = queue_family_index,
		.Queue = *queue,
		.PipelineCache = VK_NULL_HANDLE,
		.DescriptorPool = *descriptor_pool,
		.Subpass = 0,
		.MinImageCount = frame_count,
		.ImageCount = frame_count,
		.MSAASamples = VK_SAMPLE_COUNT_1_BIT,
		.Allocator = nullptr,
		.CheckVkResultFn = CheckVkResult,
	};
	ImGui_ImplVulkan_Init(&init_info, *render_pass);

	// Upload Fonts
	{
		vk::CommandBufferAllocateInfo command_buffer_info{
			.sType = vk::StructureType::eCommandBufferAllocateInfo,
			.commandPool = *command_pool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1,
		};
		vk::raii::CommandBuffers command_buffers(device, command_buffer_info);
		vk::raii::CommandBuffer command_buffer = std::move(command_buffers[0]);

		vk::CommandBufferBeginInfo begin_info {
			.sType = vk::StructureType::eCommandBufferBeginInfo,
			.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		};
		command_buffer.begin(begin_info);
		ImGui_ImplVulkan_CreateFontsTexture(*command_buffer);
		command_buffer.end();

		std::vector<vk::CommandBuffer> command_buffers_to_submit = {*command_buffer};
		vk::SubmitInfo submit_info = {
			.sType = vk::StructureType::eSubmitInfo,
			.commandBufferCount = (uint32_t)command_buffers_to_submit.size(),
			.pCommandBuffers = command_buffers_to_submit.data(),
		};
		queue.submit({submit_info}, VK_NULL_HANDLE);
		queue.waitIdle();

		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}

void Context::ImGuiShutdown() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void Context::ImGuiNewFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void Context::ImGuiRender() {
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *current_frame->command_buffer);
}

void Context::InitVkInstance() {
	const std::vector<const char*> layers = {
#ifndef NDEBUG
		"VK_LAYER_KHRONOS_validation",
#endif
	};
	const std::vector<const char*> extensions = glfw::getRequiredInstanceExtensions();

	vk::InstanceCreateInfo instance_info {
		.sType = vk::StructureType::eInstanceCreateInfo,
		.enabledLayerCount = (uint32_t)layers.size(),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = (uint32_t)extensions.size(),
		.ppEnabledExtensionNames = extensions.data(),
	};

	vk_instance = vk::raii::Instance(vk_context, instance_info);
}

void Context::InitSurface() {
	surface = vk::raii::SurfaceKHR(vk_instance, window.GetGLFW().createSurface(*vk_instance));
}

void Context::InitPhysicalDevice() {
	vk::raii::PhysicalDevices devices(vk_instance);

	std::sort(devices.begin(), devices.end(), [&](const auto& a, const auto& b) {
		return GetDeviceScore(a) > GetDeviceScore(b);
	});

	const auto it = std::find_if(devices.begin(), devices.end(), CreateDevicePredicate(surface));
	if (it == devices.end()) {
		throw std::runtime_error(__PRETTY_FUNCTION__);
	}

	physical_device = std::move(*it);

	surface_format = ChooseSurfaceFormat(physical_device, surface);
	present_mode = ChoosePresentMode(physical_device, surface);
	frame_count = ChooseImageCount(physical_device, surface);

	queue_family_index = ChooseGraphicsQueueFamilyIndex(physical_device);
}

void Context::InitDevice() {
	std::vector<float> priorities = {1.0f};

	vk::DeviceQueueCreateInfo device_queue_info {
		.sType = vk::StructureType::eDeviceQueueCreateInfo,
		.queueFamilyIndex = queue_family_index,
		.queueCount = (uint32_t)priorities.size(),
		.pQueuePriorities = priorities.data(),
	};

	std::vector<vk::DeviceQueueCreateInfo> device_queue_infos = { device_queue_info };
	std::vector<const char*> device_extensions = GetRequiredDeviceExtensions();

	vk::DeviceCreateInfo device_info {
		.sType = vk::StructureType::eDeviceCreateInfo,
		.queueCreateInfoCount = (uint32_t)device_queue_infos.size(),
		.pQueueCreateInfos = device_queue_infos.data(),
		.enabledExtensionCount = (uint32_t)device_extensions.size(),
		.ppEnabledExtensionNames = device_extensions.data(),
	};

	device = vk::raii::Device(physical_device, device_info);
}

void Context::InitQueue() {
	queue = vk::raii::Queue(device, queue_family_index, 0);
}


void Context::InitDescriptorPool() {
	vk::DescriptorPoolSize uniform_buffer_pool{
		.type = vk::DescriptorType::eUniformBuffer,
		.descriptorCount = 1000,
	};

	vk::DescriptorPoolSize combined_image_sampler_pool{
		.type = vk::DescriptorType::eCombinedImageSampler,
		.descriptorCount = 1000 + 1, // 1 for imgui
	};

	std::vector pools = {uniform_buffer_pool, combined_image_sampler_pool};

	vk::DescriptorPoolCreateInfo info{
		.sType = vk::StructureType::eDescriptorPoolCreateInfo,
		.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
		.maxSets = (2 * frame_count) + 1, // 1 for imgui
		.poolSizeCount = (uint32_t)pools.size(),
		.pPoolSizes = pools.data(),
	};

	descriptor_pool = vk::raii::DescriptorPool(device, info);
}

void Context::InitDescriptorSetLayouts() {
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		for (uint32_t i = 0; i < CBV_TABLE_SIZE; ++i) {
			vk::DescriptorSetLayoutBinding binding{
				.binding = i,
				.descriptorType = vk::DescriptorType::eUniformBuffer,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			};
			bindings.push_back(binding);
		}

		vk::DescriptorSetLayoutCreateInfo info{
			.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo,
			.bindingCount = (uint32_t)bindings.size(),
			.pBindings = bindings.data(),
		};

		cbv_set_layout = vk::raii::DescriptorSetLayout(device, info);
	}
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings;
		for (uint32_t i = 0; i < SRV_TABLE_SIZE; ++i) {
			vk::DescriptorSetLayoutBinding binding{
				.binding = i,
				.descriptorType = vk::DescriptorType::eCombinedImageSampler,
				.descriptorCount = 1,
				.stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			};
			bindings.push_back(binding);
		}
		vk::DescriptorSetLayoutCreateInfo info{
			.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo,
			.bindingCount = (uint32_t)bindings.size(),
			.pBindings = bindings.data(),
		};

		srv_set_layout = vk::raii::DescriptorSetLayout(device, info);
	}
}

void Context::InitPipelineLayout() {
	std::vector<vk::DescriptorSetLayout> set_layouts { *cbv_set_layout, *srv_set_layout };
	vk::PipelineLayoutCreateInfo pipeline_layout_info {
		.sType = vk::StructureType::ePipelineLayoutCreateInfo,
		.setLayoutCount = (uint32_t)set_layouts.size(),
		.pSetLayouts = set_layouts.data(),
	};

	pipeline_layout = vk::raii::PipelineLayout(device, pipeline_layout_info);
}

void Context::InitCommandPool() {
	vk::CommandPoolCreateInfo command_pool_info {
		.sType = vk::StructureType::eCommandPoolCreateInfo,
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = queue_family_index,
	};

	command_pool = vk::raii::CommandPool(device, command_pool_info);
}

void Context::InitNullTexture() {
	null_texture.reset(new Texture2D(4, 4));
}

void Context::InitRenderPass() {
	vk::AttachmentDescription color_attachment {
		.format = surface_format.format,
		.samples = vk::SampleCountFlagBits::e1,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
		.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
		.initialLayout = vk::ImageLayout::eUndefined,
		.finalLayout = vk::ImageLayout::ePresentSrcKHR,
	};

	vk::AttachmentReference color_attachment_ref {
		.attachment = 0,
		.layout = vk::ImageLayout::eColorAttachmentOptimal,
	};

	std::vector<vk::AttachmentReference> color_attachment_refs = {color_attachment_ref};

	vk::SubpassDescription subpass {
		.pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
		.colorAttachmentCount = (uint32_t)color_attachment_refs.size(),
		.pColorAttachments = color_attachment_refs.data(),
	};

	vk::SubpassDependency dependency {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
		.srcAccessMask = vk::AccessFlagBits::eNoneKHR,
		.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
	};

	std::vector<vk::AttachmentDescription> attachments = {color_attachment};
	std::vector<vk::SubpassDescription> subpasses = {subpass};
	std::vector<vk::SubpassDependency> dependencies = {dependency};

	vk::RenderPassCreateInfo render_pass_info {
		.sType = vk::StructureType::eRenderPassCreateInfo,
		.attachmentCount = (uint32_t)attachments.size(),
		.pAttachments = attachments.data(),
		.subpassCount = (uint32_t)subpasses.size(),
		.pSubpasses = subpasses.data(),
		.dependencyCount = (uint32_t)dependencies.size(),
		.pDependencies = dependencies.data(),
	};

	render_pass = vk::raii::RenderPass(device, render_pass_info);
}

void Context::Resize(int w, int h) {
	queue.waitIdle();
	frames.clear();
	swapchain = nullptr;
	if (w && h) {
		InitSwapchain(w, h);
		InitFrames();
	}
}

void Context::InitSwapchain(int w, int h) {
	swapchain_info = {
		.sType = vk::StructureType::eSwapchainCreateInfoKHR,
		.surface = *surface,
		.minImageCount = frame_count,
		.imageFormat = surface_format.format,
		.imageColorSpace = surface_format.colorSpace,
		.imageExtent = {
			.width = (uint32_t)w,
			.height = (uint32_t)h
		},
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = present_mode,
		.clipped = VK_TRUE,
	};

	swapchain = vk::raii::SwapchainKHR(device, swapchain_info);
}

void Context::InitNextFrameSemaphore() {
	vk::SemaphoreCreateInfo semaphore_info = {
		.sType = vk::StructureType::eSemaphoreCreateInfo,
	};
	next_frame_semaphore = vk::raii::Semaphore(device, semaphore_info);
}

void Context::InitFrames() {
	for (size_t i = 0; i < frame_count; ++i) {
		frames.push_back(Frame(i));
	}
}

Context::Context(Window& window_): window(window_) {
	g_context = this;
	auto [w, h] = window.GetWindowSize();
	InitVkInstance();
	InitSurface();
	InitPhysicalDevice();
	InitDevice();
	InitQueue();
	InitDescriptorPool();
	InitDescriptorSetLayouts();
	InitPipelineLayout();
	InitCommandPool();
	InitNullTexture();
	InitRenderPass();
	InitSwapchain(w, h);
	InitNextFrameSemaphore();
	InitFrames();

	window.OnWindowResize([&](int w, int h) {
		Resize(w, h);
	});
}

Context::~Context() {
	g_context = nullptr;
}

void Context::Clear() {
	if (!*swapchain) {
		return;
	}

	auto [result, image_index] = swapchain.acquireNextImage(UINT64_MAX, *next_frame_semaphore, nullptr);
	if (result != vk::Result::eSuccess) {
		return;
	}

	if (current_frame) {
		current_frame->Wait();
		current_frame->Reset();
	}

	current_frame = &frames.at(image_index);

	std::swap(next_frame_semaphore, current_frame->image_available_semaphore);

	current_frame->BeginFrame();
}

void Context::Present() {
	if (!*swapchain) {
		return;
	}

	current_frame->EndFrame();
	current_frame->Submit();
	current_frame->Present();
}

void Context::WaitIdle() {
	device.waitIdle();
}

vk::raii::CommandBuffer Context::BeginCommandBuffer() {
	vk::CommandBufferAllocateInfo alloc_info{
		.sType = vk::StructureType::eCommandBufferAllocateInfo,
		.commandPool = *command_pool,
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1,
	};

	vk::raii::CommandBuffer command_buffer = std::move(vk::raii::CommandBuffers(device, alloc_info)[0]);
	vk::CommandBufferBeginInfo begin_info{
		.sType = vk::StructureType::eCommandBufferBeginInfo,
		.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
	};
	command_buffer.begin(begin_info);
	return command_buffer;
}

void Context::EndCommandBuffer(vk::raii::CommandBuffer command_buffer) {
	command_buffer.end();

	std::vector<vk::CommandBuffer> command_buffers = {*command_buffer};
	vk::SubmitInfo submit_info{
		.sType = vk::StructureType::eSubmitInfo,
		.commandBufferCount = (uint32_t)command_buffers.size(),
		.pCommandBuffers = command_buffers.data(),
	};

	queue.submit({submit_info}, nullptr);
	queue.waitIdle();
}

vk::raii::DescriptorSet Context::CommitCBs() {
	std::vector<vk::DescriptorSetLayout> layouts { *cbv_set_layout };
	vk::DescriptorSetAllocateInfo alloc_info{
		.sType = vk::StructureType::eDescriptorSetAllocateInfo,
		.descriptorPool = *descriptor_pool,
		.descriptorSetCount = (uint32_t)layouts.size(),
		.pSetLayouts = layouts.data(),
	};

	vk::raii::DescriptorSet cbv_set = std::move(vk::raii::DescriptorSets(device, alloc_info)[0]);

	std::array<vk::DescriptorBufferInfo, CBV_TABLE_SIZE> buffer_info = {};
	std::array<vk::WriteDescriptorSet, CBV_TABLE_SIZE> writes = {};

	static_assert(CBV_TABLE_SIZE == UNIFORM_BUFFERS_COUNT);

	for (size_t i = 0; i < UNIFORM_BUFFERS_COUNT; ++i)
	{
		Buffer dst(UNIFORM_BUFFER_SIZE,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

		buffer_info[i] = vk::DescriptorBufferInfo{
			.buffer = *dst,
			.offset = 0,
			.range = dst.info.size,
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

		rhi::UniformBuffer& src = uniform_buffers[i];
		if (src.GetSize()) {
			void* p = dst.memory.mapMemory(0, src.GetSize());
			memcpy(p, src.GetData(), src.GetSize());
			dst.memory.unmapMemory();
			src.Reset();
		}

		current_frame->uniform_buffer_refs.push_back(std::move(dst));
	}

	device.updateDescriptorSets(writes, {});

	return cbv_set;
}

vk::raii::DescriptorSet Context::CommitSRVs() {
	std::vector<vk::DescriptorSetLayout> layouts { *srv_set_layout };
	vk::DescriptorSetAllocateInfo alloc_info{
		.sType = vk::StructureType::eDescriptorSetAllocateInfo,
		.descriptorPool = *descriptor_pool,
		.descriptorSetCount = (uint32_t)layouts.size(),
		.pSetLayouts = layouts.data(),
	};

	vk::raii::DescriptorSet srv_set = std::move(vk::raii::DescriptorSets(device, alloc_info)[0]);

	std::array<vk::DescriptorImageInfo, SRV_TABLE_SIZE> image_info = {};
	std::array<vk::WriteDescriptorSet, SRV_TABLE_SIZE> writes = {};

	for (size_t i = 0; i < SRV_TABLE_SIZE; ++i)
	{
		image_info[i] = vk::DescriptorImageInfo{
			.sampler = *null_texture->sampler,
			.imageView = *null_texture->image_view,
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

	device.updateDescriptorSets(writes, {});

	return srv_set;
}

void Context::CommitAll() {
	vk::raii::DescriptorSet cbv_set = CommitCBs();
	vk::raii::DescriptorSet srv_set = CommitSRVs();
	current_frame->command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline_layout, 0, {*cbv_set, *srv_set}, {});
	current_frame->descriptor_set_refs.push_back(std::move(cbv_set));
	current_frame->descriptor_set_refs.push_back(std::move(srv_set));
}

}
