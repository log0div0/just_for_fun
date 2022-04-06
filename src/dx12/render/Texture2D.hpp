
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

	winapi::ComPtr<ID3D12Resource> texture_buffer;
	winapi::ComPtr<ID3D12DescriptorHeap> heap;
	D3D12_CPU_DESCRIPTOR_HANDLE texture_srv_handle;
};

}
