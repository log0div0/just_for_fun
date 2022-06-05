#include "CommandQueue.hpp"
#include "Exceptions.hpp"
#include "../Context.hpp"

using namespace winapi;

namespace dx12 {

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) {
	D3D12_COMMAND_QUEUE_DESC desc = {
		.Type = type,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 1,
	};
	ThrowIfFailed(g_context->device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue)));
	ThrowIfFailed(g_context->device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

void CommandQueue::Execute(winapi::ComPtr<ID3D12GraphicsCommandList>& command_list) {
	command_queue->ExecuteCommandLists(1, (ID3D12CommandList* const *)&command_list);
}

void CommandQueue::ExecuteSync(winapi::ComPtr<ID3D12GraphicsCommandList>& command_list) {
	Execute(command_list);
	Signal();
	WaitIdle();
}

void CommandQueue::WaitForFenceValue(uint64_t fence_value) {
	if (fence_value == 0) {
		return;
	}
	if (fence->GetCompletedValue() >= fence_value) {
		return;
	}
	fence->SetEventOnCompletion(fence_value, fence_event.handle);
	fence_event.Wait(INFINITE);
}

void CommandQueue::WaitIdle() {
	WaitForFenceValue(fence_counter);
}

uint64_t CommandQueue::Signal() {
	command_queue->Signal(fence, ++fence_counter);
	return fence_counter;
}

}
