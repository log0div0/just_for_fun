#pragma once

#include <winapi/ComPtr.hpp>

#include <d3d12.h>

namespace rhi {

struct Frame {
	Frame() = default;
	Frame(int frame_index_);

	void Begin();
	void End();

	int frame_index = -1;
	winapi::ComPtr<ID3D12Resource> render_target_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE render_target_desc;
	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
	uint64_t fence_value = 0;
};

}
