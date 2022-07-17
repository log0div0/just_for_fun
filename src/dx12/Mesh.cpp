
#include "Mesh.hpp"
#include "Context.hpp"

#include "../Vertex.hpp"

#include <winapi/Functions.hpp>

using namespace winapi;

namespace dx12 {

Mesh::Mesh(const fs::path& path) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::tie(vertices, indices) = LoadFromFile(path);

	size_t vertex_buffer_size = vertices.size()*sizeof(vertices[0]);
	size_t index_buffer_size = indices.size()*sizeof(indices[0]);
	size_t buffer_size = vertex_buffer_size + index_buffer_size;

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

		THROW_IF_FAILED(g_context->device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&buffer_desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_GRAPHICS_PPV_ARGS(&buffer)));
	}

	winapi::ComPtr<ID3D12Resource> upload_buffer;

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC buffer_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_size);

		THROW_IF_FAILED(g_context->device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&buffer_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_GRAPHICS_PPV_ARGS(&upload_buffer)));
	}

	{
		uint8_t* data = nullptr;
		THROW_IF_FAILED(upload_buffer->Map(0, nullptr, (void**)&data));
		memcpy(data, vertices.data(), vertex_buffer_size);
		memcpy(data+vertex_buffer_size, indices.data(), index_buffer_size);
		upload_buffer->Unmap(0, nullptr);
	}

	{
		winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
		winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
		THROW_IF_FAILED(g_context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_GRAPHICS_PPV_ARGS(&command_allocator)));
		THROW_IF_FAILED(g_context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, command_allocator, NULL, IID_GRAPHICS_PPV_ARGS(&command_list)));

		command_list->CopyBufferRegion(buffer, 0, upload_buffer, 0, buffer_size);

		command_list->Close();

		g_context->copy_queue.ExecuteSync(command_list);
	}
	{
		winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
		winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
		THROW_IF_FAILED(g_context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_GRAPHICS_PPV_ARGS(&command_allocator)));
		THROW_IF_FAILED(g_context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, IID_GRAPHICS_PPV_ARGS(&command_list)));

		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			buffer,
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ
		);
		command_list->ResourceBarrier(1, &barrier);
		command_list->Close();

		g_context->direct_queue.ExecuteSync(command_list);

	}

	vertex_buffer_view = {
		.BufferLocation = buffer->GetGPUVirtualAddress(),
		.SizeInBytes = (UINT)vertex_buffer_size,
		.StrideInBytes = sizeof(vertices[0]),
	};

	index_buffer_view = {
		.BufferLocation = buffer->GetGPUVirtualAddress() + vertex_buffer_size,
		.SizeInBytes = (UINT)index_buffer_size,
		.Format = DXGI_FORMAT_R32_UINT,
	};
}

void Mesh::Draw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	g_context->CommitAll();

	g_context->command_list->SetPipelineState(shader.pipeline_state);

	g_context->command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_context->command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view);
	g_context->command_list->IASetIndexBuffer(&index_buffer_view);

	uint32_t index_count = index_buffer_view.SizeInBytes / sizeof(uint32_t);

	g_context->command_list->DrawIndexedInstanced(index_count, 1, 0, 0, 0);
}

}
