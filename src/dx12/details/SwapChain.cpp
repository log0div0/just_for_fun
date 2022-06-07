
#include "SwapChain.hpp"
#include "Exceptions.hpp"
#include "../Context.hpp"

using namespace winapi;

#ifndef _GAMING_XBOX
#include <glfwpp/native.h>
#endif

namespace dx12 {

SwapChain::SwapChain(uint32_t w, uint32_t h, uint32_t buffers_count) {
	ComPtr<IDXGIFactory5> dxgi_factory;
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory)));
	// ThrowIfFailed(dxgi_factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &has_tearing_support, sizeof(has_tearing_support)));

	UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (has_tearing_support ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);

	DXGI_SWAP_CHAIN_DESC1 sd =
	{
		.Width = w,
		.Height = h,
		.Format = FORMAT,
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
	ThrowIfFailed(dxgi_factory->CreateSwapChainForHwnd(g_context->direct_queue.Get(), glfw::native::getWin32Window(g_context->window.GetGLFW()), &sd, NULL, NULL, &swap_chain1));
	swap_chain = swap_chain1.QueryInterface<IDXGISwapChain3>();
	ThrowIfFailed(swap_chain->SetMaximumFrameLatency(buffers_count));
	waitable_object = winapi::Object(swap_chain->GetFrameLatencyWaitableObject());
}

winapi::ComPtr<ID3D12Resource> SwapChain::GetBuffer(uint32_t buffer_index) {
	winapi::ComPtr<ID3D12Resource> buffer;
	ThrowIfFailed(swap_chain->GetBuffer(buffer_index, IID_PPV_ARGS(&buffer)));
	return buffer;
}

void SwapChain::ResizeBuffers(uint32_t w, uint32_t h) {
	UINT flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT | (has_tearing_support ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0);
	ThrowIfFailed(swap_chain->ResizeBuffers(
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

}
