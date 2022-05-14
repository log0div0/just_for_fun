#pragma once

#include "DescriptorHeap.hpp"

namespace render {

struct DescriptorTable {
	DescriptorTable() = default;
	DescriptorTable(DescriptorHeap* heap, size_t size);
	~DescriptorTable();

	DescriptorTable(const DescriptorTable& other) = delete;
	DescriptorTable& operator=(const DescriptorTable& other) = delete;

	DescriptorTable(DescriptorTable&& other);
	DescriptorTable& operator=(DescriptorTable&& other);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(size_t index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetBaseGPUHandle() const;

	size_t GetSize() const {
		return size;
	}

private:
	DescriptorHeap* heap = nullptr;
	DescriptorPair base_handle = {};
	size_t size = 0;
};

}
