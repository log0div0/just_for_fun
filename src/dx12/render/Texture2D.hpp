
#pragma once

#include <winapi/ComPtr.hpp>

#include <d3d12.h>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

namespace render {

struct Texture2D {
	Texture2D() = default;
	Texture2D(const fs::path& path);
	~Texture2D();

	Texture2D(const Texture2D& other) = delete;
	Texture2D& operator=(const Texture2D& other) = delete;

	Texture2D(Texture2D&& other);
	Texture2D& operator=(Texture2D&& other);

	winapi::ComPtr<ID3D12Resource> resource;
	static const DXGI_FORMAT img_format = DXGI_FORMAT_R8G8B8A8_UNORM;
};

}
