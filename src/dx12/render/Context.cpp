#include "Context.hpp"
#include "Exceptions.hpp"

#include <d3dx12.h>

using namespace winapi;

namespace render {

Frame::Frame(int frame_index)
{
	ThrowIfFailed(context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)));

	InitRenderTargetBuffer(frame_index);
}

void Frame::InitRenderTargetBuffer(int frame_index) {
	render_target_buffer = context->swap_chain.GetBuffer(frame_index);
	render_target_desc = context->rtv_desc_heap->GetCPUDescriptorHandleForHeapStart();
	size_t rtv_descriptor_size = context->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	render_target_desc.ptr += rtv_descriptor_size * frame_index;
	context->device->CreateRenderTargetView(render_target_buffer, NULL, render_target_desc);
}

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

void Context::InitHeaps()
{
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE,
		};
		ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srv_desc_heap)));
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = NUM_FRAMES_IN_FLIGHT,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
			.NodeMask = 1,
		};
		ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtv_desc_heap)));
	}
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		};
		ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&dsv_desc_heap)));
	}
}

void Context::InitQueues()
{
	direct_queue = CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	copy_queue = CommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
}

void Context::InitSwapchain(int w, int h)
{
	swap_chain = SwapChain(w, h, NUM_FRAMES_IN_FLIGHT);
	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
	scissor_rect = CD3DX12_RECT(0, 0, w, h);
}

void Context::InitDepthStencilBuffer(int w, int h) {
	depth_stencil_desc = dsv_desc_heap->GetCPUDescriptorHandleForHeapStart();

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, w, h,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		D3D12_CLEAR_VALUE clear_value = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.DepthStencil = { 1.0f, 0 },
		};

		ThrowIfFailed(device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clear_value,
			IID_PPV_ARGS(&depth_stencil_buffer)
		));
	}

	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
			.Texture2D = {
				.MipSlice = 0,
			},
		};


		device->CreateDepthStencilView(depth_stencil_buffer, &desc, depth_stencil_desc);
	}
}

void Context::InitFrames()
{
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i] = Frame(i);
	}
}

Context* context = nullptr;

Context::Context(glfw::Window& window_): window(window_) {
	context = this;
	auto [w, h] = window.getFramebufferSize();
	InitDevice();
	InitHeaps();
	InitQueues();
	InitSwapchain(w, h);
	InitDepthStencilBuffer(w, h);
	InitFrames();
	window.framebufferSizeEvent.subscribe([&](glfw::Window& window, int w, int h) {
		Resize(w, h);
	});
}

Context::~Context() {
	context = nullptr;
}

void Context::Resize(int w, int h) {
	if (!w || !h) {
		return;
	}
	WaitIdle();
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i].render_target_buffer = {};
	}
	depth_stencil_buffer = {};
	swap_chain.ResizeBuffers(w, h);
	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
	scissor_rect = CD3DX12_RECT(0, 0, w, h);
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i].InitRenderTargetBuffer(i);
	}
	InitDepthStencilBuffer(w, h);
}

void Context::WaitIdle() {
	direct_queue.WaitIdle();
	copy_queue.WaitIdle();
}

void Context::Clear() {
	int frame_idx = swap_chain.AcquireNextBufferIndex();
	current_frame = &frames[frame_idx];
	direct_queue.WaitForFenceValue(current_frame->fence_value);

	current_frame->command_allocator->Reset();
	if (command_list.IsNull()) {
		ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, current_frame->command_allocator, NULL, IID_PPV_ARGS(&command_list)));
	} else {
		command_list->Reset(current_frame->command_allocator, NULL);
	}

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		current_frame->render_target_buffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	command_list->ResourceBarrier(1, &barrier);

	const float clear_color[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	float depth = 1.0f;

	command_list->ClearRenderTargetView(current_frame->render_target_desc, clear_color, 0, NULL);
	command_list->ClearDepthStencilView(depth_stencil_desc, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	command_list->OMSetRenderTargets(1, &current_frame->render_target_desc, FALSE, &depth_stencil_desc);
	command_list->SetDescriptorHeaps(1, &srv_desc_heap);
	command_list->RSSetViewports(1, &viewport);
	command_list->RSSetScissorRects(1, &scissor_rect);
}

void Context::Present() {
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		current_frame->render_target_buffer,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();

	direct_queue.Execute(command_list);
	swap_chain.Present();
	current_frame->fence_value = direct_queue.Signal();
}

}
