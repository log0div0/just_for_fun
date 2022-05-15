#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace render {

struct Frame {
	Frame();

	vk::raii::Semaphore image_available_semaphore = nullptr;
	vk::raii::Semaphore render_finished_semaphore = nullptr;
	vk::raii::Fence fence = nullptr;
};

}
