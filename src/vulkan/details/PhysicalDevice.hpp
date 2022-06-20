
#pragma once

#include <vulkan/vulkan_raii.hpp>

std::vector<const char*> GetRequiredDeviceExtensions();
bool DoesDeviceSupportRequiredExtensions(const vk::raii::PhysicalDevice& device);

vk::SurfaceFormatKHR ChooseSurfaceFormat(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface);
vk::PresentModeKHR ChoosePresentMode(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface);
uint32_t ChooseImageCount(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface);

uint32_t ChooseGraphicsQueueFamilyIndex(const vk::raii::PhysicalDevice& device);

size_t GetDeviceScore(const vk::raii::PhysicalDevice& device);
std::function<bool(const vk::raii::PhysicalDevice&)> CreateDevicePredicate(const vk::raii::SurfaceKHR& surface);

uint32_t FindMemoryType(const vk::raii::PhysicalDevice& device, uint32_t memory_types, vk::MemoryPropertyFlags memory_props);