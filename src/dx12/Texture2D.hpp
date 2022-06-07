
#pragma once

#include "../rhi/Texture2D.hpp"

#include "details/Platform.hpp"

namespace dx12 {

struct Texture2D: rhi::Texture2D {
	Texture2D(const fs::path& path);
	virtual ~Texture2D() override {}

	winapi::ComPtr<ID3D12Resource> resource;
	static const DXGI_FORMAT img_format = DXGI_FORMAT_R8G8B8A8_UNORM;
};

}
