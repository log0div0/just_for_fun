#include "Context.hpp"
#include "Exceptions.hpp"

#include <glfwpp/native.h>

using namespace winapi;

namespace rhi {

void Context::InitDevice()
{
	ComPtr<ID3D12Debug> debug;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
	debug->EnableDebugLayer();

	ThrowIfFailed(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));

	ComPtr<ID3D12InfoQueue> info_queue = device.QueryInterface<ID3D12InfoQueue>();
	ThrowIfFailed(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
	ThrowIfFailed(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));
	ThrowIfFailed(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true));
}

void Context::InitShaderResourceViews()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		.NumDescriptors = 1,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
	};
	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srv_desc_heap)));
}

void Context::InitCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 1,
	};
	ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue)));
}

void Context::InitFence()
{
	ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

void Context::InitSwapchain()
{
	ComPtr<IDXGIFactory5> dxgi_factory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory)));
	ThrowIfFailed(dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &has_tearing_support, sizeof(has_tearing_support)));

	UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (has_tearing_support ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

	DXGI_SWAP_CHAIN_DESC1 sd =
	{
		.Width = 0,
		.Height = 0,
		.Format = RTV_FORMAT,
		.Stereo = FALSE,
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = NUM_FRAMES_IN_FLIGHT,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = flags,
	};
	ComPtr<IDXGISwapChain1> swap_chain1;
	ThrowIfFailed(dxgi_factory->CreateSwapChainForHwnd(command_queue, glfw::native::getWin32Window(window), &sd, NULL, NULL, &swap_chain1));
	swap_chain = swap_chain1.QueryInterface<IDXGISwapChain3>();
	ThrowIfFailed(swap_chain->SetMaximumFrameLatency(NUM_FRAMES_IN_FLIGHT));
	swap_chain_waitable_object = winapi::Object(swap_chain->GetFrameLatencyWaitableObject());
}

Frame::Frame(int frame_index_): frame_index(frame_index_)
{
	ThrowIfFailed(context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)));
	ThrowIfFailed(context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, IID_PPV_ARGS(&command_list)));
	ThrowIfFailed(command_list->Close());

	ThrowIfFailed(context->swap_chain->GetBuffer(frame_index, IID_PPV_ARGS(&render_target_resource)));
	render_target_desc = context->rtv_desc_heap->GetCPUDescriptorHandleForHeapStart();
	size_t rtv_descriptor_size = context->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	render_target_desc.ptr += rtv_descriptor_size * frame_index;
	context->device->CreateRenderTargetView(render_target_resource, NULL, render_target_desc);
}

void Context::InitFrames()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {
		.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
		.NumDescriptors = NUM_FRAMES_IN_FLIGHT,
		.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		.NodeMask = 1,
	};
	ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtv_desc_heap)));

	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i] = Frame(i);
	}
}

Context* context = nullptr;

Context::Context(glfw::Window& window_): window(window_) {
	context = this;
	InitDevice();
	InitShaderResourceViews();
	InitCommandQueue();
	InitFence();
	InitSwapchain();
	InitFrames();
	window.framebufferSizeEvent.subscribe([&](glfw::Window& window, int w, int h) {
		ResizeBackBuffers(w, h);
	});
}

Context::~Context() {
	context = nullptr;
}

void Context::ResizeBackBuffers(int w, int h) {
	WaitIdle();
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i] = {};
	}
	UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (has_tearing_support ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
    ThrowIfFailed(swap_chain->ResizeBuffers(
    	0, w, h,
    	DXGI_FORMAT_UNKNOWN,
    	flags
    ));
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i] = Frame(i);
	}
}

void Context::WaitIdle() {
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		WaitForFenceValue(frames[i].fence_value);
	}
}

void Context::WaitForFenceValue(uint64_t fence_value) {
	if (fence_value == 0) {
		return;
	}
	if (fence->GetCompletedValue() >= fence_value) {
		return;
	}
	fence->SetEventOnCompletion(fence_value, fence_event.handle);
	fence_event.Wait(INFINITE);
}

void Frame::Begin() {
	command_allocator->Reset();

	D3D12_RESOURCE_BARRIER barrier = {
		.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
		.Transition = {
			.pResource   = render_target_resource,
			.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			.StateBefore = D3D12_RESOURCE_STATE_PRESENT,
			.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET,
		},
	};
	command_list->Reset(command_allocator, NULL);
	command_list->ResourceBarrier(1, &barrier);

	const float clear_color[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	command_list->ClearRenderTargetView(render_target_desc, clear_color, 0, NULL);
	command_list->OMSetRenderTargets(1, &render_target_desc, FALSE, NULL);
	command_list->SetDescriptorHeaps(1, &context->srv_desc_heap);
}

void Context::Clear() {
	swap_chain_waitable_object.Wait(INFINITE);
	int frame_idx = swap_chain->GetCurrentBackBufferIndex();
	current_frame = &frames[frame_idx];
	WaitForFenceValue(current_frame->fence_value);
	current_frame->Begin();
}

void Frame::End() {
	D3D12_RESOURCE_BARRIER barrier = {
		.Type  = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
		.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE,
		.Transition = {
			.pResource   = render_target_resource,
			.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
			.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET,
			.StateAfter  = D3D12_RESOURCE_STATE_PRESENT,
		},
	};
	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();
}

void Context::Present() {
	current_frame->End();
	current_frame->fence_value = ++fence_counter;

	command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const *)&current_frame->command_list);

	swap_chain->Present(0, has_tearing_support ? DXGI_PRESENT_ALLOW_TEARING : 0);

	command_queue->Signal(fence, current_frame->fence_value);
}

}
