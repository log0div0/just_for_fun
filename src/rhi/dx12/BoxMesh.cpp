
#include "BoxMesh.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"
#include "../Vertex.hpp"

using namespace winapi;

namespace rhi {

winapi::ComPtr<ID3D12Resource> CreateBuffer(size_t buffer_size, D3D12_HEAP_TYPE heap_type, D3D12_RESOURCE_STATES resource_state) {
	D3D12_HEAP_PROPERTIES heap_props = {
		.Type = heap_type,
		.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN,
		.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN,
		.CreationNodeMask = 1,
		.VisibleNodeMask = 1,
	};

	D3D12_RESOURCE_DESC buffer_desc = {
		.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
		.Alignment = 0,
		.Width = buffer_size,
		.Height = 1,
		.DepthOrArraySize = 1,
		.MipLevels = 1,
		.Format = DXGI_FORMAT_UNKNOWN,
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
		.Flags = D3D12_RESOURCE_FLAG_NONE,
	};

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

	uint8_t* dst = nullptr;
	ThrowIfFailed(upload_buffer->Map(0, nullptr, (void**)&dst));
	memcpy(dst, box_vertices.data(), buffer_size);
	upload_buffer->Unmap(0, nullptr);

	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
	ThrowIfFailed(context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&command_allocator)));
	ThrowIfFailed(context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, command_allocator, NULL, IID_PPV_ARGS(&command_list)));
	command_list->CopyBufferRegion(default_buffer, 0, upload_buffer, 0, buffer_size);
	command_list->Close();

	context->copy_queue.ExecuteSync(command_list);

	vertex_buffer = std::move(default_buffer);
}

void BoxMesh::Draw() {

}

}
