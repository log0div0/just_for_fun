#include "DescriptorTable.hpp"

namespace dx12 {

DescriptorTable::DescriptorTable(DescriptorHeap* heap_, size_t size_): heap(heap_), size(size_) {
	base_handle = heap->alloc(size);
}

DescriptorTable::~DescriptorTable() {
	if (heap && size) {
		heap->free(base_handle, size);
	}
}

DescriptorTable::DescriptorTable(DescriptorTable&& other):
	heap(other.heap),
	base_handle(other.base_handle),
	size(other.size)
{
	other.heap = nullptr;
	other.size = 0;
}

DescriptorTable& DescriptorTable::operator=(DescriptorTable&& other) {
	std::swap(heap, other.heap);
	std::swap(base_handle, other.base_handle);
	std::swap(size, other.size);
	return *this;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorTable::GetCPUHandle(size_t index) const {
	D3D12_CPU_DESCRIPTOR_HANDLE result = base_handle.cpu;
	result.ptr += index * heap->descriptor_size;
	return result;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorTable::GetBaseGPUHandle() const {
	return base_handle.gpu;
}

}
