#include "Frame.hpp"
#include "Image.hpp"
#include "../Context.hpp"

namespace render {

Frame::Frame() {
	vk::SemaphoreCreateInfo semaphore_info = {
		.sType = vk::StructureType::eSemaphoreCreateInfo,
	};
	image_available_semaphore = vk::raii::Semaphore(g_context->device, semaphore_info);
	render_finished_semaphore = vk::raii::Semaphore(g_context->device, semaphore_info);

	vk::FenceCreateInfo fence_info = {
		.sType = vk::StructureType::eFenceCreateInfo,
	};
	fence = vk::raii::Fence(g_context->device, fence_info);
}

}
