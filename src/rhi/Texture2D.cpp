#include "Texture2D.hpp"
#include "../Utils.hpp"

namespace rhi
{

stb::Image Texture2D::LoadFromFile(const fs::path& path) {
	return stb::Image(GetAssetsDir() / "textures" / path, 4);
}

}
