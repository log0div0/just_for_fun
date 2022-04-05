
#pragma once

#include <winapi/ComPtr.hpp>

#include <d3d12.h>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

namespace render {

struct Texture2D {
	Texture2D() = default;
	Texture2D(const fs::path& path);
	void Bind(uint32_t unit);

private:
	winapi::ComPtr<ID3D12Resource> texture_buffer;
	D3D12_CPU_DESCRIPTOR_HANDLE texture_srv_handle;
};

}
