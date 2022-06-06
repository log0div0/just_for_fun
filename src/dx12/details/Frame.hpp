
#pragma once

#include <winapi/ComPtr.hpp>

#include "DescriptorTable.hpp"
#include "ConstantBuffer.hpp"

namespace dx12 {

struct Frame {
	Frame() = default;
	Frame(int frame_index);

	void InitRenderTargetBuffer(int frame_index);

	winapi::ComPtr<ID3D12Resource> render_target_buffer;
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle;
	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	uint64_t fence_value = 0;
	int index = -1;

	std::vector<DescriptorTable> descriptor_table_refs;
	std::vector<ConstantBuffer> constant_buffers_refs;
};

}
