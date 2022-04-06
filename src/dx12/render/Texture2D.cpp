#include "Texture2D.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"

#include <stb/Image.hpp>

#include <d3dx12.h>

using namespace winapi;

namespace render {

Texture2D::Texture2D(const fs::path& path) {
	stb::Image img(path, 4);

	DXGI_FORMAT img_format = DXGI_FORMAT_R8G8B8A8_UNORM;

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
			IID_PPV_ARGS(&texture_buffer)));
	}

	ComPtr<ID3D12Resource> upload_buffer;

	{
		UINT64 required_size = GetRequiredIntermediateSize(texture_buffer, 0, 1);

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

	UpdateSubresources(command_list, texture_buffer, upload_buffer, 0, 0, 1, &subresource_data);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture_buffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();

	g_context->direct_queue.ExecuteSync(command_list);

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			.NumDescriptors = 1,
			.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
		};
		ThrowIfFailed(g_context->device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap)));
	}

	texture_srv_handle = heap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = img_format;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.MostDetailedMip = 0;
	g_context->device->CreateShaderResourceView(texture_buffer, &srv_desc, texture_srv_handle);
}

Texture2D::~Texture2D() {

}

Texture2D::Texture2D(Texture2D&& other):
	texture_buffer(std::move(other.texture_buffer)),
	heap(std::move(other.heap)),
	texture_srv_handle(std::move(other.texture_srv_handle))
{
}

Texture2D& Texture2D::operator=(Texture2D&& other) {
	std::swap(texture_buffer, other.texture_buffer);
	std::swap(heap, other.heap);
	std::swap(texture_srv_handle, other.texture_srv_handle);
	return *this;
}

}
