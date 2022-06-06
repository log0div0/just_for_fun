#pragma once

#include "../rhi/Texture2D.hpp"

#ifdef WIN32
#include <Windows.h>
#endif

#include <glad/glad.h>
#include <mogl/mogl.hpp>

namespace opengl {

struct Texture2D: rhi::Texture2D {
	Texture2D(const fs::path& path);
	mogl::Texture2D texture;
};

}
