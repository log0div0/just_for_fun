#pragma once

#include "SwapChain.hpp"
#include "CommandQueue.hpp"

#include <glfwpp/glfwpp.h>
#include <array>

namespace rhi {

// PSO - pipeline state object

// IA - input assembly
// VS - vertex shader
// HS - hull shader
// DS - domain shader
// GS - geometry shader
// SO - stream output
// RS - rasterizer stage
// PS - pixel shader
// OM - output merger

// RTV - render target view
// SRV - shader resource view
// UAV - unordered access view
// CBV - constant buffer view
// DSV - depth stencil view

struct Frame {
	Frame() = default;
	Frame(int frame_index);

	void InitRenderTargetBuffer(int frame_index);

	winapi::ComPtr<ID3D12Resource> render_target_buffer;
	D3D12_CPU_DESCRIPTOR_HANDLE render_target_desc;
	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
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
	winapi::ComPtr<ID3D12Device2> device;

	void InitHeaps();
	winapi::ComPtr<ID3D12DescriptorHeap> srv_desc_heap;
	winapi::ComPtr<ID3D12DescriptorHeap> rtv_desc_heap;
	winapi::ComPtr<ID3D12DescriptorHeap> dsv_desc_heap;

	void InitQueues();
	CommandQueue direct_queue;
	CommandQueue copy_queue;

	void InitSwapchain(int w, int h);
	SwapChain swap_chain;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissor_rect;

	void InitDepthStencilBuffer(int w, int h);
	winapi::ComPtr<ID3D12Resource> depth_stencil_buffer;
	D3D12_CPU_DESCRIPTOR_HANDLE depth_stencil_desc;

	void InitFrames();
	std::array<Frame, NUM_FRAMES_IN_FLIGHT> frames = {};
	Frame* current_frame = nullptr;

	void InitCommandList();
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;

	void Resize(int w, int h);
};

extern Context* context;

}
