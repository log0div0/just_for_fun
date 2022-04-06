
#pragma once

#include <winapi/ComPtr.hpp>

#include <d3d12.h>

#include <bitset>

namespace render {

struct DescriptorPair {
	D3D12_CPU_DESCRIPTOR_HANDLE	cpu;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu;
};

struct DescriptorHeap {
	static constexpr size_t heap_size = 1024;

	DescriptorHeap() = default;
	DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);
	~DescriptorHeap();

	DescriptorPair alloc();
	DescriptorPair alloc_range(int num);

	winapi::ComPtr<ID3D12DescriptorHeap> heap;
	std::bitset<heap_size> allocation_map = {};
	size_t cursor = 0;
	size_t increment_size = 0;
	bool shader_visible = false;
};

}
