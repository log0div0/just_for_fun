
#pragma once

#include "../../Utils.hpp"

#include <winapi/ComPtr.hpp>
#include <winapi/Event.hpp>

#include <d3d12.h>
#include <dxgi1_6.h>

namespace rhi {

struct SwapChain {
	static inline const DXGI_FORMAT FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

	SwapChain() = default;
	SwapChain(glfw::Window& window, winapi::ComPtr<ID3D12CommandQueue>& command_queue, uint32_t buffers_count);

	SwapChain(SwapChain&& other) = default;
	SwapChain& operator=(SwapChain&& other) = default;

	winapi::ComPtr<ID3D12Resource> GetBuffer(uint32_t buffer_index);
	void ResizeBuffers(uint32_t w, uint32_t h);
	uint32_t AcquireNextBufferIndex();
	void Present();

	winapi::ComPtr<IDXGISwapChain3> swap_chain;
	winapi::Object waitable_object;
	BOOL has_tearing_support = false;
};

}
