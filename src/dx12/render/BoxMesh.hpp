
#pragma once

#include <winapi/ComPtr.hpp>

#include <d3d12.h>

namespace render {

struct BoxMesh {
	BoxMesh();

	void Draw();

	winapi::ComPtr<ID3D12Resource> vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	// winapi::ComPtr<ID3D12Resource> index_buffer;
	// D3D12_INDEX_BUFFER_VIEW index_view;
};

}
