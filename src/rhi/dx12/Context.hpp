#pragma once

#include "SwapChain.hpp"
#include "CommandQueue.hpp"
#include "Frame.hpp"

#include <glfwpp/glfwpp.h>

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

	void InitHeaps();
	winapi::ComPtr<ID3D12DescriptorHeap> srv_desc_heap;
	winapi::ComPtr<ID3D12DescriptorHeap> rtv_desc_heap;
	winapi::ComPtr<ID3D12DescriptorHeap> dsv_desc_heap;

	void InitQueues();
	CommandQueue direct_queue;
	CommandQueue copy_queue;

	void InitSwapchain();
	SwapChain swap_chain;

	void InitFrames();
	Frame frames[NUM_FRAMES_IN_FLIGHT] = {};
	Frame* current_frame = nullptr;

	void ResizeBackBuffers(int w, int h);
};

extern Context* context;

}
