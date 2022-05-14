
#pragma once

#include <winapi/ComPtr.hpp>

#include <d3d12.h>

#include <vector>

namespace render {

struct DescriptorPair {
	D3D12_CPU_DESCRIPTOR_HANDLE	cpu;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu;
};

struct DescriptorHeap {
	DescriptorHeap() = default;
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t size);
	~DescriptorHeap();

	DescriptorPair alloc(int num = 1);
	void free(DescriptorPair base, int num = 1);

	winapi::ComPtr<ID3D12DescriptorHeap> heap;
	std::vector<bool> allocation_map;
	size_t cursor = 0;
	size_t descriptor_size = 0;
	bool shader_visible = false;
	size_t heap_size = 0;
};

}
