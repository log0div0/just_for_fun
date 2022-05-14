#pragma once

#include "details/SwapChain.hpp"
#include "details/CommandQueue.hpp"
#include "details/DescriptorHeap.hpp"
#include "details/DescriptorTable.hpp"
#include "details/Frame.hpp"
#include "details/ConstantBuffer.hpp"
#include "Texture2D.hpp"

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

enum { NUM_FRAMES_IN_FLIGHT = 3 };

enum {
	SRV_TABLE_INDEX,
	CBV_TABLE_INDEX,
	SAMPLER_TABLE_INDEX,
	NUM_OF_ROOT_SIG_TABLES
};

enum {
	SRV_TABLE_SIZE = 8,
	CBV_TABLE_SIZE = 8,
	SAMPLER_TABLE_SIZE = 4
};

enum {
	MAX_CB_SIZE = 1024
};

enum {
	VIEW_HEAP_SIZE = 10000 + 1, // 1 for imgui
	SAMPLER_HEAP_SIZE = 1000,
	RTV_HEAP_SIZE = NUM_FRAMES_IN_FLIGHT,
	DSV_HEAP_SIZE = 1
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

	void InitDevice();
	winapi::ComPtr<ID3D12Device2> device;

	void InitRootSignature();
	winapi::ComPtr<ID3D12RootSignature> root_signature;

	void InitHeaps();
	DescriptorHeap view_heap;
	DescriptorHeap sampler_heap;
	DescriptorHeap rtv_heap;
	DescriptorHeap dsv_heap;

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

	void InitCBs();
	void CommitCBs();
	std::array<ConstantBuffer, CBV_TABLE_SIZE> constant_buffers = {};

	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;

	void Resize(int w, int h);
	void CommitResources();

	void CreateSRV(size_t root_parameter_index, Texture2D& texture);
private:
	void CommitSRVs();
	DescriptorTable srv_table;
	std::array<bool, SRV_TABLE_SIZE> srv_table_map = {};

	void CommitSamplers();
	DescriptorTable sampler_table;
};

extern Context* g_context;

}
