#pragma once

#include <stb/Image.hpp>

#include <glad/glad.h>
#include <mogl/mogl.hpp>

namespace render {

struct Texture2D {
	Texture2D() = default;

	Texture2D(const fs::path& path)
	{
		stb::Image img(path);
		texture.setStorage(5, GL_RGBA8, img.w, img.h);
		texture.setSubImage(0, 0, 0, img.w, img.h, img.c == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, img.data);
		texture.generateMipmap();
	}

	mogl::Texture2D texture;
};

}
