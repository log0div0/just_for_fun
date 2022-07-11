
#include "SwapChain.hpp"
#include "../Context.hpp"

#include <winapi/Functions.hpp>

using namespace winapi;

#ifndef _GAMING_XBOX
#include <glfwpp/native.h>
#endif

namespace dx12 {

#ifdef _GAMING_XBOX

SwapChain::SwapChain(uint32_t w, uint32_t h, uint32_t buffers_count) {
	CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);

	D3D12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Tex2D(
	    BACK_BUFFER_FORMAT,
	    w,
	    h,
	    1, // This resource has only one texture.
	    1  // Use a single mipmap level.
	);
	buffer_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = BACK_BUFFER_FORMAT;

	for (UINT n = 0; n < buffers_count; n++)
	{
		ComPtr<ID3D12Resource> buffer;

	    THROW_IF_FAILED(g_context->device->CreateCommittedResource(
	        &heap_props,
	        D3D12_HEAP_FLAG_ALLOW_DISPLAY,
	        &buffer_desc,
	        D3D12_RESOURCE_STATE_PRESENT,
	        &clear_value,
	        IID_GRAPHICS_PPV_ARGS(&buffer)));

	    wchar_t name[25] = {};
	    swprintf_s(name, L"Back buffer %u", n);
	    buffer->SetName(name);

	    back_buffers.push_back(std::move(buffer));
	}

	RegisterFrameEvents();
}

// Set frame interval and register for frame events
void SwapChain::RegisterFrameEvents()
{
    // First, retrieve the underlying DXGI device from the D3D device.
    ComPtr<IDXGIDevice1> dxgiDevice;
    THROW_IF_FAILED(g_context->device->QueryInterface(IID_GRAPHICS_PPV_ARGS(&dxgiDevice)));

    // Identify the physical adapter (GPU or card) this device is running on.
    ComPtr<IDXGIAdapter> dxgiAdapter;
    THROW_IF_FAILED(dxgiDevice->GetAdapter(&dxgiAdapter));

    // Retrieve the outputs for the adapter.
    ComPtr<IDXGIOutput> dxgiOutput;
    THROW_IF_FAILED(dxgiAdapter->EnumOutputs(0, &dxgiOutput));

    // Set frame interval and register for frame events
    THROW_IF_FAILED(g_context->device->SetFrameIntervalX(
        dxgiOutput.Get(),
        D3D12XBOX_FRAME_INTERVAL_60_HZ,
        back_buffers.size() - 1u /* Allow n-1 frames of latency */,
        D3D12XBOX_FRAME_INTERVAL_FLAG_NONE));

    THROW_IF_FAILED(g_context->device->ScheduleFrameEventX(
        D3D12XBOX_FRAME_EVENT_ORIGIN,
        0U,
        nullptr,
        D3D12XBOX_SCHEDULE_FRAME_EVENT_FLAG_NONE));
}

winapi::ComPtr<ID3D12Resource> SwapChain::GetBuffer(uint32_t buffer_index) {
	return back_buffers.at(buffer_index);
}

void SwapChain::ResizeBuffers(uint32_t w, uint32_t h) {
	throw std::runtime_error("Should be never called on Xbox?");
}

uint32_t SwapChain::AcquireNextBufferIndex() {
	frame_pipeline_token = D3D12XBOX_FRAME_PIPELINE_TOKEN_NULL;
	THROW_IF_FAILED(g_context->device->WaitFrameEventX(D3D12XBOX_FRAME_EVENT_ORIGIN, INFINITE, nullptr, D3D12XBOX_WAIT_FRAME_EVENT_FLAG_NONE, &frame_pipeline_token));

	current_back_buffer = (current_back_buffer + 1) % back_buffers.size();
	return current_back_buffer;
}

void SwapChain::Present() {
	D3D12XBOX_PRESENT_PARAMETERS* params = nullptr;
	D3D12XBOX_PRESENT_PLANE_PARAMETERS plane_params = {
		.Token = frame_pipeline_token,
		.ResourceCount = 1,
		.ppResources = &back_buffers.at(current_back_buffer),
	};
	THROW_IF_FAILED(g_context->direct_queue.Get()->PresentX(1, &plane_params, params));
}

#else

SwapChain::SwapChain(uint32_t w, uint32_t h, uint32_t buffers_count) {
	ComPtr<IDXGIFactory5> dxgi_factory;
	THROW_IF_FAILED(CreateDXGIFactory1(IID_GRAPHICS_PPV_ARGS(&dxgi_factory)));
	// THROW_IF_FAILED(dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &has_tearing_support, sizeof(has_tearing_support)));

	UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (has_tearing_support ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

	DXGI_SWAP_CHAIN_DESC1 sd =
	{
		.Width = w,
		.Height = h,
		.Format = BACK_BUFFER_FORMAT,
		.Stereo = FALSE,
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = buffers_count,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = flags,
	};
	ComPtr<IDXGISwapChain1> swap_chain1;
	THROW_IF_FAILED(dxgi_factory->CreateSwapChainForHwnd(g_context->direct_queue.Get(), glfw::native::getWin32Window(g_context->window.GetGLFW()), &sd, NULL, NULL, &swap_chain1));
	swap_chain = swap_chain1.QueryInterface<IDXGISwapChain3>();
	THROW_IF_FAILED(swap_chain->SetMaximumFrameLatency(buffers_count));
	waitable_object = winapi::Object(swap_chain->GetFrameLatencyWaitableObject());
}

winapi::ComPtr<ID3D12Resource> SwapChain::GetBuffer(uint32_t buffer_index) {
	winapi::ComPtr<ID3D12Resource> buffer;
	THROW_IF_FAILED(swap_chain->GetBuffer(buffer_index, IID_GRAPHICS_PPV_ARGS(&buffer)));
	return buffer;
}

void SwapChain::ResizeBuffers(uint32_t w, uint32_t h) {
	UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (has_tearing_support ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
	THROW_IF_FAILED(swap_chain->ResizeBuffers(
		0, w, h,
		DXGI_FORMAT_UNKNOWN,
		flags
	));
}

uint32_t SwapChain::AcquireNextBufferIndex() {
	waitable_object.Wait(INFINITE);
	return swap_chain->GetCurrentBackBufferIndex();
}

void SwapChain::Present() {
	swap_chain->Present(0, has_tearing_support ? DXGI_PRESENT_ALLOW_TEARING : 0);
}

#endif

}
