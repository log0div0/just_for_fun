
#include "BoxMesh.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"
#include "../Vertex.hpp"

#include <d3dx12.h>

using namespace winapi;

namespace rhi {

winapi::ComPtr<ID3D12Resource> CreateBuffer(size_t buffer_size, D3D12_HEAP_TYPE heap_type, D3D12_RESOURCE_STATES resource_state) {
	CD3DX12_HEAP_PROPERTIES heap_props(heap_type);
	CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

	winapi::ComPtr<ID3D12Resource> buffer;

	ThrowIfFailed(context->device->CreateCommittedResource(
	    &heap_props,
	    D3D12_HEAP_FLAG_NONE,
	    &buffer_desc,
	    resource_state,
	    nullptr,
	    IID_PPV_ARGS(&buffer)));

	return buffer;
}

BoxMesh::BoxMesh() {
	size_t buffer_size = box_vertices.size() * sizeof(Vertex);

	winapi::ComPtr<ID3D12Resource> default_buffer = CreateBuffer(buffer_size, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);
	winapi::ComPtr<ID3D12Resource> upload_buffer = CreateBuffer(buffer_size, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ);

	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
	ThrowIfFailed(context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&command_allocator)));
	ThrowIfFailed(context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, command_allocator, NULL, IID_PPV_ARGS(&command_list)));

	D3D12_SUBRESOURCE_DATA subresource_data = {
	    .pData = box_vertices.data(),
	    .RowPitch = (LONG_PTR)buffer_size,
	    .SlicePitch = (LONG_PTR)buffer_size,
	};

    UpdateSubresources(command_list, default_buffer, upload_buffer, 0, 0, 1, &subresource_data);

	command_list->Close();

	context->copy_queue.ExecuteSync(command_list);

	vertex_buffer = std::move(default_buffer);
	vertex_buffer_view = {
		.BufferLocation = vertex_buffer->GetGPUVirtualAddress(),
		.SizeInBytes = (UINT)buffer_size,
		.StrideInBytes = sizeof(Vertex),
	};
}

void BoxMesh::Draw() {
	context->command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
	// context->command_list->IASetIndexBuffer(&index_buffer_view);

	context->command_list->DrawInstanced(box_vertices.size(), 1, 0, 0);
}

}
