
#include "PhysicalDevice.hpp"
#include <set>

std::vector<const char*> GetRequiredDeviceExtensions() {
	return {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		VK_KHR_MAINTENANCE_1_EXTENSION_NAME
	};
};

bool DoesDeviceSupportRequiredExtensions(const vk::raii::PhysicalDevice& device) {
	std::vector<const char*> device_extensions = GetRequiredDeviceExtensions();
	std::set<std::string> required_extensions(device_extensions.begin(), device_extensions.end());
	for (const vk::ExtensionProperties& extension: device.enumerateDeviceExtensionProperties()) {
		required_extensions.erase(extension.extensionName);
	}
	return required_extensions.empty();
};

vk::SurfaceFormatKHR ChooseSurfaceFormat(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {
	const std::vector<vk::SurfaceFormatKHR> availableFormats = device.getSurfaceFormatsKHR(*surface);
	for (const auto& availableFormat: availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return availableFormat;
		}
	}
	return availableFormats.at(0);
}

vk::PresentModeKHR ChoosePresentMode(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {
	for (const vk::PresentModeKHR& mode: device.getSurfacePresentModesKHR(*surface)) {
		if (mode == vk::PresentModeKHR::eFifo) {
			return mode;
		}
	}
	throw std::runtime_error(__PRETTY_FUNCTION__);
}

uint32_t ChooseImageCount(const vk::raii::PhysicalDevice& device, const vk::raii::SurfaceKHR& surface) {
	const vk::SurfaceCapabilitiesKHR capabilities = device.getSurfaceCapabilitiesKHR(*surface);
	return std::clamp(3u, capabilities.minImageCount, capabilities.maxImageCount);
}

uint32_t ChooseGraphicsQueueFamilyIndex(const vk::raii::PhysicalDevice& device) {
	const auto families = device.getQueueFamilyProperties();
	for (uint32_t i = 0; i < families.size(); ++i) {
		if (families[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			return i;
		}
	}
	throw std::runtime_error(__PRETTY_FUNCTION__);
}


uint32_t FindMemoryType(const vk::raii::PhysicalDevice& device, uint32_t memory_types, vk::MemoryPropertyFlags memory_props) {
	vk::PhysicalDeviceMemoryProperties properties = device.getMemoryProperties();
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
		if ((memory_types & (1 << i)) && (properties.memoryTypes[i].propertyFlags & memory_props) == memory_props) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}



size_t GetDeviceScore(const vk::raii::PhysicalDevice& device) {
	size_t score = 0;
	if (device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
		score += 1000;
	}
	return score;
}

std::function<bool(const vk::raii::PhysicalDevice&)> CreateDevicePredicate(const vk::raii::SurfaceKHR& surface) {
	return [&surface](const vk::raii::PhysicalDevice& device) -> bool {
		try {
			if (!DoesDeviceSupportRequiredExtensions(device)) {
				return false;
			}
			ChooseSurfaceFormat(device, surface);
			ChoosePresentMode(device, surface);
			const uint32_t index = ChooseGraphicsQueueFamilyIndex(device);
			if (!device.getSurfaceSupportKHR(index, *surface)) {
				return false;
			}
		} catch (const std::exception&) {
			return false;
		}
		return true;
	};
}
