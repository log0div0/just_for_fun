#include "Frame.hpp"
#include "../Context.hpp"
#include "Exceptions.hpp"
#include <cassert>

using namespace winapi;

namespace dx12 {

Frame::Frame(int frame_index): index(frame_index)
{
	ThrowIfFailed(g_context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_GRAPHICS_PPV_ARGS(&command_allocator)));
	rtv_handle = g_context->rtv_heap.alloc().cpu;

	InitRenderTargetBuffer(frame_index);
}

void Frame::InitRenderTargetBuffer(int frame_index) {
	assert(index == frame_index);
	render_target_buffer = g_context->swap_chain.GetBuffer(frame_index);
	g_context->device->CreateRenderTargetView(render_target_buffer, NULL, rtv_handle);
}

}
