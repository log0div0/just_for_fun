#pragma once

#include <stb/Image.hpp>

#include "../Utils.hpp"

#include "../rhi/Texture2D.hpp"

namespace vulkan {

struct Texture2D: rhi::Texture2D {
	Texture2D(const fs::path& path)
	{
	}
};

}
