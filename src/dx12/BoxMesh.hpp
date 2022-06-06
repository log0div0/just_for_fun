
#pragma once

#include "../rhi/BoxMesh.hpp"
#include "ShaderProgram.hpp"

namespace dx12 {

struct BoxMesh: rhi::BoxMesh {
	BoxMesh();

	virtual void DoDraw(rhi::ShaderProgram& shader) override;

	winapi::ComPtr<ID3D12Resource> vertex_buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view;
	// winapi::ComPtr<ID3D12Resource> index_buffer;
	// D3D12_INDEX_BUFFER_VIEW index_view;
};

}
