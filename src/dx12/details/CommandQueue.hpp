
#pragma once

#include "Platform.hpp"

namespace dx12 {

struct CommandQueue {
	CommandQueue() = default;

	CommandQueue(D3D12_COMMAND_LIST_TYPE type);
	void Execute(winapi::ComPtr<ID3D12GraphicsCommandList>& command_list);
	void ExecuteSync(winapi::ComPtr<ID3D12GraphicsCommandList>& command_list);
	uint64_t Signal();
	void WaitForFenceValue(uint64_t fence_value);
	void WaitIdle();

	winapi::ComPtr<ID3D12CommandQueue> Get() {
		return command_queue;
	}

	winapi::ComPtr<ID3D12CommandQueue> command_queue;
	winapi::ComPtr<ID3D12Fence> fence;
	winapi::Event fence_event;
	uint64_t fence_counter = 0;
};

}
