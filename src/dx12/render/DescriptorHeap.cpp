#include "DescriptorHeap.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"

namespace render {

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) {
	D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if ((type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)) {
		shader_visible = true;
		flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}
	D3D12_DESCRIPTOR_HEAP_DESC desc = {
		.Type = type,
		.NumDescriptors = heap_size,
		.Flags = flags,
	};
	ThrowIfFailed(g_context->device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));

	increment_size = g_context->device->GetDescriptorHandleIncrementSize(type);
}

DescriptorHeap::~DescriptorHeap() {
	// check allocation_map for all descriptors to be freed?
}

DescriptorPair DescriptorHeap::alloc() {
	size_t initial_cursor = cursor;
	do
	{
		if (allocation_map.test(cursor)) {
			cursor = (cursor + 1) % heap_size;
			continue;
		}
		allocation_map.set(cursor);

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap->GetCPUDescriptorHandleForHeapStart();
		cpu_handle.ptr += increment_size * cursor;

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = {};
		if (shader_visible) {
			gpu_handle = heap->GetGPUDescriptorHandleForHeapStart();
			gpu_handle.ptr += increment_size * cursor;
		}

		return {cpu_handle, gpu_handle};
	} while (initial_cursor != cursor);

	throw std::runtime_error("Failed to allocate descriptor");
}

DescriptorPair DescriptorHeap::alloc_range(int num) {
	size_t initial_cursor = cursor;
	do
	{
		bool is_ok = true;
		for (int i = 0; i < num; ++i) {
			if (allocation_map.test(cursor + i)) {
				is_ok = false;
				break;
			}
		}
		if (!is_ok) {
			cursor = (cursor + 1) % heap_size;
			continue;
		}
		for (int i = 0; i < num; ++i) {
			allocation_map.set(cursor + i);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap->GetCPUDescriptorHandleForHeapStart();
		cpu_handle.ptr += increment_size * cursor;

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = {};
		if (shader_visible) {
			gpu_handle = heap->GetGPUDescriptorHandleForHeapStart();
			gpu_handle.ptr += increment_size * cursor;
		}

		return {cpu_handle, gpu_handle};
	} while (initial_cursor != cursor);

	throw std::runtime_error("Failed to allocate descriptor");
}

}
