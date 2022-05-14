#include "ConstantBuffer.hpp"
#include "../Context.hpp"
#include <stdexcept>
#include <cstdlib>

#include <d3dx12.h>

void* aligned_alloc(size_t alignment, size_t size) {
#ifdef WIN32
	return _aligned_malloc(size, alignment);
#else
	return std::aligned_alloc(alignment, size);
#endif
}

void aligned_free(void* p) {
#ifdef WIN32
	_aligned_free(p);
#else
	std::free(p);
#endif
}

namespace render {

ConstantBufferMemory::ConstantBufferMemory(const uint8_t* data, size_t size) {
	// TODO: use mem pool here
	if (size == 0) {
		return;
	}
	CD3DX12_HEAP_PROPERTIES heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC res_desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	g_context->device->CreateCommittedResource(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&res_desc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));

	void* p = nullptr;
	resource->Map(0, nullptr, &p);
	memcpy(p, data, size);
	resource->Unmap(0, nullptr);
}

ConstantBufferMemory::ConstantBufferMemory(ConstantBufferMemory&& other): resource(std::move(other.resource)) {
}

ConstantBufferMemory& ConstantBufferMemory::operator=(ConstantBufferMemory&& other) {
	std::swap(resource, other.resource);
	return *this;
}

D3D12_GPU_VIRTUAL_ADDRESS ConstantBufferMemory::GetGPUVirtualAddress() {
	return resource.Get() ? resource->GetGPUVirtualAddress() : 0;
}

UINT ConstantBufferMemory::GetSize() {
	return resource.Get() ? resource->GetDesc().Width : 0;
}

ConstantBuffer::ConstantBuffer(size_t size): max_size(size) {
	assert(size % 256 == 0);
	shadow_data = (uint8_t*)aligned_alloc(16, max_size);
}

ConstantBuffer::~ConstantBuffer() {
	if (shadow_data) {
		aligned_free(shadow_data);
	}
}

ConstantBuffer& ConstantBuffer::operator=(ConstantBuffer&& other) {
	std::swap(shadow_data, other.shadow_data);
	std::swap(max_size, other.max_size);
	std::swap(current_size, other.current_size);
	return *this;
}

void ConstantBuffer::Write(size_t offset, const uint8_t* src, int num_rows, int src_stride, int dst_stride) {
	size_t end_of_write = offset + num_rows*dst_stride;
	if (end_of_write > max_size) {
		throw std::runtime_error("Out of bounds while writing to the constant buffer");
	}
	uint8_t* dst = shadow_data + offset;
	for (int i = 0; i < num_rows; ++i) {
		memcpy(dst + i*dst_stride, src + i*src_stride, src_stride);
	}
	current_size = std::max(current_size, end_of_write);
}

size_t RoundUp(size_t n, size_t round_to) {
	return ((n + round_to-1) / round_to) * round_to;
}

ConstantBufferMemory ConstantBuffer::CommitToDevice() {
	size_t aligned_size = RoundUp(current_size, 256);
	ConstantBufferMemory mem(shadow_data, aligned_size);
	current_size = 0;
	return mem;
}

}
