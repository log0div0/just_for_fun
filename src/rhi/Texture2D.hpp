
#pragma once

#include "Context.hpp"

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <stb/Image.hpp>

namespace rhi
{

struct Texture2D {
	static stb::Image LoadFromFile(const fs::path& path);

	virtual ~Texture2D() = 0 {}
};

}
