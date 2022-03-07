#pragma once

#include "../../Utils.hpp"

#include <winapi/ComPtr.hpp>
#include <winapi/Event.hpp>

#include <d3d12.h>
#include <dxgi1_6.h>

namespace rhi {

// RTV - render target view
// SRV - shader resource view
// UAV - unordered access view
// CBV - constant buffer view
// DSV - depth stencil view

struct Context;

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

struct Context {
	Context(glfw::Window& window_);
	~Context();

	Context(const Context&) = delete;
	Context& operator=(const Context&) = delete;

	Context(Context&&) = delete;
	Context& operator=(Context&&) = delete;

	glfw::Window& window;

	void Clear();
	void Present();
	void WaitIdle();

	enum { NUM_FRAMES_IN_FLIGHT = 3 };
	static inline const DXGI_FORMAT RTV_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

	void InitDevice();
	winapi::ComPtr<ID3D12Device> device;

	void InitShaderResourceViews();
	winapi::ComPtr<ID3D12DescriptorHeap> srv_desc_heap;

	void InitCommandQueue();
	winapi::ComPtr<ID3D12CommandQueue> command_queue;

	void InitFence();
	winapi::ComPtr<ID3D12Fence> fence;
	winapi::Event fence_event;
	uint64_t fence_counter = 0;

	void InitSwapchain();
	winapi::ComPtr<IDXGISwapChain3> swap_chain;
	winapi::Object swap_chain_waitable_object;
	BOOL has_tearing_support = false;

	void InitFrames();
	winapi::ComPtr<ID3D12DescriptorHeap> rtv_desc_heap;
	Frame frames[NUM_FRAMES_IN_FLIGHT] = {};
	Frame* current_frame = nullptr;

	void WaitForFenceValue(uint64_t fence_value);
	void ResizeBackBuffers(int w, int h);
};

extern Context* context;

}
