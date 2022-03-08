#include "Frame.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"

#include <d3dx12.h>

using namespace winapi;

namespace rhi {

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

void Frame::Begin() {
	command_allocator->Reset();
	command_list->Reset(command_allocator, NULL);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		render_target_resource,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	command_list->ResourceBarrier(1, &barrier);

	const float clear_color[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	command_list->ClearRenderTargetView(render_target_desc, clear_color, 0, NULL);
	command_list->OMSetRenderTargets(1, &render_target_desc, FALSE, NULL);
	command_list->SetDescriptorHeaps(1, &context->srv_desc_heap);
}

void Frame::End() {
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		render_target_resource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();
}

}
