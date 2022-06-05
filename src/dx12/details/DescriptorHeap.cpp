#include "DescriptorHeap.hpp"
#include "Exceptions.hpp"
#include "../Context.hpp"

namespace dx12 {

DescriptorHeap::DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t size): heap_size(size) {
	D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if ((type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)) {
		shader_visible = true;
		flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	}
	D3D12_DESCRIPTOR_HEAP_DESC desc = {
		.Type = type,
		.NumDescriptors = (UINT)heap_size,
		.Flags = flags,
	};
	ThrowIfFailed(g_context->device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));

	descriptor_size = g_context->device->GetDescriptorHandleIncrementSize(type);
	allocation_map.resize(heap_size, false);
}

DescriptorHeap::~DescriptorHeap() {
	// check allocation_map for all descriptors to be freed?
}

DescriptorPair DescriptorHeap::alloc(int num) {
	size_t initial_cursor = cursor;
	do
	{
		bool is_ok = true;
		for (int i = 0; i < num; ++i) {
			if ((cursor + i) >= heap_size) {
				is_ok = false;
				break;
			}
			if (allocation_map[cursor + i]) {
				is_ok = false;
				break;
			}
		}
		if (!is_ok) {
			cursor = (cursor + 1) % heap_size;
			continue;
		}
		for (int i = 0; i < num; ++i) {
			allocation_map[cursor + i] = true;
		}

		D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap->GetCPUDescriptorHandleForHeapStart();
		cpu_handle.ptr += descriptor_size * cursor;

		D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = {};
		if (shader_visible) {
			gpu_handle = heap->GetGPUDescriptorHandleForHeapStart();
			gpu_handle.ptr += descriptor_size * cursor;
		}

		return {cpu_handle, gpu_handle};
	} while (initial_cursor != cursor);

	throw std::runtime_error("Failed to allocate descriptor");
}

void DescriptorHeap::free(DescriptorPair base_handle, int num) {
	D3D12_CPU_DESCRIPTOR_HANDLE start = heap->GetCPUDescriptorHandleForHeapStart();
	int base_pos = (base_handle.cpu.ptr - start.ptr) / descriptor_size;
	for (int i = 0; i < num; ++i) {
		assert(allocation_map[base_pos + i]);
		allocation_map[base_pos + i] = false;
	}
}

}
