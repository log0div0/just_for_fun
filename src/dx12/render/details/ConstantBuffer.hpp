
#pragma once

#include <winapi/ComPtr.hpp>
#include <d3d12.h>

#include <memory>
#include <optional>

namespace render {

struct ConstantBufferMemory {
	ConstantBufferMemory(const uint8_t* data, size_t size);

	ConstantBufferMemory(const ConstantBufferMemory& other) = delete;
	ConstantBufferMemory& operator=(const ConstantBufferMemory& other) = delete;

	ConstantBufferMemory(ConstantBufferMemory&& other);
	ConstantBufferMemory& operator=(ConstantBufferMemory&& other);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();
	UINT GetSize();

private:
	winapi::ComPtr<ID3D12Resource> resource;
};

struct ConstantBuffer {
	ConstantBuffer() = default;
	ConstantBuffer(size_t size);
	~ConstantBuffer();

	ConstantBuffer(const ConstantBuffer& other) = delete;
	ConstantBuffer& operator=(const ConstantBuffer& other) = delete;

	ConstantBuffer(ConstantBuffer&& other);
	ConstantBuffer& operator=(ConstantBuffer&& other);

	void Write(size_t offset, const uint8_t* src, int num_rows, int src_stride, int dst_stride);

	ConstantBufferMemory CommitToDevice();

private:
	uint8_t* shadow_data = nullptr;
	size_t max_size = 0;
	size_t current_size = 0;
};

}
