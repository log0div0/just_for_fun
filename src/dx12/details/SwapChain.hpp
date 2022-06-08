
#pragma once

#include "CommandQueue.hpp"
#include "Platform.hpp"
#include <vector>

namespace dx12 {

struct SwapChain {
	static inline const DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

	SwapChain() = default;
	SwapChain(uint32_t w, uint32_t h, uint32_t buffers_count);

	SwapChain(SwapChain&& other) = default;
	SwapChain& operator=(SwapChain&& other) = default;

	winapi::ComPtr<ID3D12Resource> GetBuffer(uint32_t buffer_index);
	void ResizeBuffers(uint32_t w, uint32_t h);
	uint32_t AcquireNextBufferIndex();
	void Present();

#ifdef _GAMING_XBOX
	void RegisterFrameEvents();
	std::vector<winapi::ComPtr<ID3D12Resource>> back_buffers;
	int current_back_buffer = -1;
	D3D12XBOX_FRAME_PIPELINE_TOKEN frame_pipeline_token = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
#else
	winapi::ComPtr<IDXGISwapChain3> swap_chain;
	winapi::Object waitable_object;
	BOOL has_tearing_support = false;
#endif
};

}
