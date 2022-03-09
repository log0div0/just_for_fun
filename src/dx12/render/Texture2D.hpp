
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

namespace render {

struct Texture2D {
	Texture2D() = default;
	Texture2D(const fs::path& path);
	void Bind(uint32_t unit);
};

}
