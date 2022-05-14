#include "Texture2D.hpp"
#include "details/Exceptions.hpp"
#include "Context.hpp"

#include <stb/Image.hpp>

#include <d3dx12.h>

using namespace winapi;

namespace render {

Texture2D::Texture2D(const fs::path& path) {
	stb::Image img(path, 4);

	int mips_num = 1;

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(img_format, img.w, img.h, 1, mips_num);

		ThrowIfFailed(g_context->device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&resource)));
	}

	ComPtr<ID3D12Resource> upload_buffer;

	{
		UINT64 required_size = GetRequiredIntermediateSize(resource, 0, 1);

		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(required_size);

		ThrowIfFailed(g_context->device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&upload_buffer)));
	}

	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
	ThrowIfFailed(g_context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)));
	ThrowIfFailed(g_context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, IID_PPV_ARGS(&command_list)));

	D3D12_SUBRESOURCE_DATA subresource_data = {
	    .pData = img.data,
	    .RowPitch = (LONG_PTR)img.stride(),
	    .SlicePitch = (LONG_PTR)img.data_len(),
	};

	UpdateSubresources(command_list, resource, upload_buffer, 0, 0, 1, &subresource_data);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();

	g_context->direct_queue.ExecuteSync(command_list);
}

Texture2D::~Texture2D() {

}

Texture2D::Texture2D(Texture2D&& other):
	resource(std::move(other.resource))
{
}

Texture2D& Texture2D::operator=(Texture2D&& other) {
	std::swap(resource, other.resource);
	return *this;
}

}
