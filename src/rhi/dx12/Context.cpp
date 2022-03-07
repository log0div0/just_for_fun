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
	direct_queue = CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
}

void Context::InitSwapchain()
{
	swap_chain = SwapChain(NUM_FRAMES_IN_FLIGHT);
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
	direct_queue.WaitIdle();
}

void Context::Clear() {
	int frame_idx = swap_chain.AcquireNextBufferIndex();
	current_frame = &frames[frame_idx];
	direct_queue.WaitForFenceValue(current_frame->fence_value);
	current_frame->Begin();
}

void Context::Present() {
	current_frame->End();
	direct_queue.Execute(current_frame->command_list);
	swap_chain.Present();
	current_frame->fence_value = direct_queue.Signal();
}

}
