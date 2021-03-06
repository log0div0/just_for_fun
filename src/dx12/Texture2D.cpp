#include "Texture2D.hpp"
#include "Context.hpp"

#include <stb/Image.hpp>
#include <winapi/Functions.hpp>

using namespace winapi;

namespace dx12 {

Texture2D::Texture2D(const fs::path& path) {
	img_format = DXGI_FORMAT_R8G8B8A8_UNORM;

	stb::Image img = LoadFromFile(path);

	int mips_num = 1;

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(img_format, img.w, img.h, 1, mips_num);

		THROW_IF_FAILED(g_context->device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_GRAPHICS_PPV_ARGS(&img_buffer)));
	}

	ComPtr<ID3D12Resource> upload_buffer;

	{
		UINT64 required_size = GetRequiredIntermediateSize(img_buffer, 0, 1);

		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_UPLOAD);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(required_size);

		THROW_IF_FAILED(g_context->device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_GRAPHICS_PPV_ARGS(&upload_buffer)));
	}

	winapi::ComPtr<ID3D12CommandAllocator> command_allocator;
	winapi::ComPtr<ID3D12GraphicsCommandList> command_list;
	THROW_IF_FAILED(g_context->device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_GRAPHICS_PPV_ARGS(&command_allocator)));
	THROW_IF_FAILED(g_context->device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator, NULL, IID_GRAPHICS_PPV_ARGS(&command_list)));

	D3D12_SUBRESOURCE_DATA subresource_data = {
	    .pData = img.data,
	    .RowPitch = (LONG_PTR)img.stride(),
	    .SlicePitch = (LONG_PTR)img.data_len(),
	};

	UpdateSubresources(command_list.Get(), img_buffer, upload_buffer, 0, 0, 1, &subresource_data);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(img_buffer,
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE|D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();

	g_context->direct_queue.ExecuteSync(command_list);
}

}
