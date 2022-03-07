#pragma once

#include "SwapChain.hpp"
#include "CommandQueue.hpp"

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

	void InitDevice();
	winapi::ComPtr<ID3D12Device> device;

	void InitShaderResourceViews();
	winapi::ComPtr<ID3D12DescriptorHeap> srv_desc_heap;

	void InitCommandQueue();
	CommandQueue direct_queue;

	void InitSwapchain();
	SwapChain swap_chain;

	void InitFrames();
	winapi::ComPtr<ID3D12DescriptorHeap> rtv_desc_heap;
	Frame frames[NUM_FRAMES_IN_FLIGHT] = {};
	Frame* current_frame = nullptr;

	void ResizeBackBuffers(int w, int h);
};

extern Context* context;

}
