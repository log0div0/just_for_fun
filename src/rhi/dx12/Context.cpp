#include "Context.hpp"
#include "Exceptions.hpp"

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
	swap_chain = SwapChain(window, command_queue, NUM_FRAMES_IN_FLIGHT);
}

Frame::Frame(int frame_index_): frame_index(frame_index_)
{
	ThrowIfFailed(context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)));
	ThrowIfFailed(context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, IID_PPV_ARGS(&command_list)));
	ThrowIfFailed(command_list->Close());

	render_target_resource = context->swap_chain.GetBuffer(frame_index);
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
	swap_chain.ResizeBuffers(w, h);
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
	int frame_idx = swap_chain.AcquireNextBufferIndex();
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

	swap_chain.Present();

	command_queue->Signal(fence, current_frame->fence_value);
}

}
