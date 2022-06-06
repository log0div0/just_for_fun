
#include "BoxMesh.hpp"
#include "Context.hpp"

#include "details/Exceptions.hpp"
#include "../Vertex.hpp"

#include <d3dx12.h>

using namespace winapi;

namespace dx12 {

BoxMesh::BoxMesh() {
	size_t buffer_size = box_vertices.size() * sizeof(Vertex);

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

		ThrowIfFailed(g_context->device->CreateCommittedResource(
		    &heap_props,
		    D3D12_HEAP_FLAG_NONE,
		    &buffer_desc,
		    D3D12_RESOURCE_STATE_COPY_DEST,
		    nullptr,
		    IID_PPV_ARGS(&vertex_buffer)));
	}

	winapi::ComPtr<ID3D12Resource> upload_buffer;

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

		ThrowIfFailed(g_context->device->CreateCommittedResource(
		    &heap_props,
		    D3D12_HEAP_FLAG_NONE,
		    &buffer_desc,
		    D3D12_RESOURCE_STATE_GENERIC_READ,
		    nullptr,
		    IID_PPV_ARGS(&upload_buffer)));
	}

	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
	ThrowIfFailed(g_context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&command_allocator)));
	ThrowIfFailed(g_context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, command_allocator, NULL, IID_PPV_ARGS(&command_list)));

	D3D12_SUBRESOURCE_DATA subresource_data = {
	    .pData = box_vertices.data(),
	    .RowPitch = (LONG_PTR)buffer_size,
	    .SlicePitch = (LONG_PTR)buffer_size,
	};

    UpdateSubresources(command_list, vertex_buffer, upload_buffer, 0, 0, 1, &subresource_data);

	command_list->Close();

	g_context->copy_queue.ExecuteSync(command_list);

	vertex_buffer_view = {
		.BufferLocation = vertex_buffer->GetGPUVirtualAddress(),
		.SizeInBytes = (UINT)buffer_size,
		.StrideInBytes = sizeof(Vertex),
	};
}

void BoxMesh::DoDraw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	g_context->command_list->SetPipelineState(shader.pipeline_state);

	g_context->command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_context->command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
	// g_context->command_list->IASetIndexBuffer(&index_buffer_view);

	g_context->command_list->DrawInstanced(box_vertices.size(), 1, 0, 0);
}

}
