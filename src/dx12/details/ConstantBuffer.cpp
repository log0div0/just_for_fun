#include "ConstantBuffer.hpp"
#include "../Context.hpp"
#include <stdexcept>
#include <cstdlib>

#include <d3dx12.h>

namespace dx12 {

ConstantBuffer::ConstantBuffer(const rhi::UniformBuffer& ub) {
	// TODO: use mem pool here
	if (ub.GetSize() == 0) {
		return;
	}
	CD3DX12_HEAP_PROPERTIES heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC res_desc = CD3DX12_RESOURCE_DESC::Buffer(ub.GetSize());
	g_context->device->CreateCommittedResource(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&res_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	void* p = nullptr;
	resource->Map(0, nullptr, &p);
	memcpy(p, ub.GetData(), ub.GetSize());
	resource->Unmap(0, nullptr);
}

ConstantBuffer::ConstantBuffer(ConstantBuffer&& other): resource(std::move(other.resource)) {
}

ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer&& other) {
	std::swap(resource, other.resource);
	return *this;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBuffer::GetGPUVirtualAddress() {
	return resource.Get() ? resource->GetGPUVirtualAddress() : 0;
}

UINT ConstantBuffer::GetSize() {
	return resource.Get() ? resource->GetDesc().Width : 0;
}

}
