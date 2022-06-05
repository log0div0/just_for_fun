
#pragma once

#include "Context.hpp"

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

namespace rhi
{

struct Texture2D {
	virtual ~Texture2D() = 0 {}
};

}
