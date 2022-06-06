
#pragma once

#include <winapi/ComPtr.hpp>
#include <d3d12.h>

#include <memory>
#include <optional>

#include "../../rhi/Context.hpp"

namespace dx12 {

struct ConstantBuffer {
	ConstantBuffer(const rhi::UniformBuffer& ub);

	ConstantBuffer(const ConstantBuffer& other) = delete;
	ConstantBuffer& operator=(const ConstantBuffer& other) = delete;

	ConstantBuffer(ConstantBuffer&& other);
	ConstantBuffer& operator=(ConstantBuffer&& other);

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress();
	UINT GetSize();

private:
	winapi::ComPtr<ID3D12Resource> resource;
};

}
