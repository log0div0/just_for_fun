#pragma once

#include "SwapChain.hpp"
#include "CommandQueue.hpp"
#include "DescriptorHeap.hpp"
#include "Frame.hpp"

#include <glfwpp/glfwpp.h>
#include <array>

namespace render {

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

	void InitDevice();
	winapi::ComPtr<ID3D12Device2> device;

	void InitHeaps();
	DescriptorHeap srv_desc_heap;
	DescriptorHeap rtv_desc_heap;
	DescriptorHeap dsv_desc_heap;

	void InitQueues();
	CommandQueue direct_queue;
	CommandQueue copy_queue;

	void InitSwapchain(int w, int h);
	SwapChain swap_chain;
    D3D12_VIEWPORT viewport;
    D3D12_RECT scissor_rect;

	void InitDepthStencilBuffer(int w, int h);
	winapi::ComPtr<ID3D12Resource> depth_stencil_buffer;
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle;

	void InitFrames();
	std::array<Frame, NUM_FRAMES_IN_FLIGHT> frames = {};
	Frame* current_frame = nullptr;

	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;

	void Resize(int w, int h);
};

extern Context* g_context;

}
