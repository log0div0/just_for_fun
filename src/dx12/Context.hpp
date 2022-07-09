#pragma once

#include "details/SwapChain.hpp"
#include "details/CommandQueue.hpp"
#include "details/DescriptorHeap.hpp"
#include "details/DescriptorTable.hpp"
#include "details/Frame.hpp"
#include "Texture2D.hpp"
#include "ShaderProgram.hpp"
#include "BoxMesh.hpp"

#include "../Window.hpp"

#include <array>

namespace dx12 {

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
	CBV_TABLE_SIZE = UNIFORM_BUFFERS_COUNT,
	SAMPLER_TABLE_SIZE = 4
};

enum {
	VIEW_HEAP_SIZE = 10000 + 1, // 1 for imgui
	SAMPLER_HEAP_SIZE = 1000,
	RTV_HEAP_SIZE = NUM_FRAMES_IN_FLIGHT,
	DSV_HEAP_SIZE = 1
};

struct Context: rhi::Context {
	Context(Window& window_);
	virtual ~Context() override;

	Window& window;

	virtual rhi::Texture2D* CreateTexture2D(const fs::path& path) override { return new Texture2D(path); }
	virtual rhi::ShaderProgram* CreateShaderProgram(const std::string& name) override { return new ShaderProgram(name); }
	virtual rhi::BoxMesh* CreateBoxMesh() override { return new BoxMesh(); }

	virtual void Clear() override;
	virtual void Present() override;
	virtual void WaitIdle() override;

#ifndef _GAMING_XBOX
	virtual void ImGuiInit() override;
	virtual void ImGuiShutdown() override;
	virtual void ImGuiNewFrame() override;
	virtual void ImGuiRender() override;
#endif

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

    void InitDepthStencilHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = {};

	void InitDepthStencilTexture(int w, int h);
	Texture2D depth_stencil_texture;

	void InitFrames();
	std::array<Frame, NUM_FRAMES_IN_FLIGHT> frames = {};
	Frame* current_frame = nullptr;

	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;

	void Resize(int w, int h);

	void CommitAll();

	void SetSRV(size_t root_parameter_index, Texture2D& texture);
private:
	void CommitCBs();

	void CommitSRVs();
	DescriptorTable srv_table;
	std::array<bool, SRV_TABLE_SIZE> srv_table_map = {};

	void CommitSamplers();
	DescriptorTable sampler_table;
};

extern Context* g_context;

}
