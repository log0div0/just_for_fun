
#pragma once

#include "../rhi/Mesh.hpp"
#include "ShaderProgram.hpp"

namespace dx12 {

struct Mesh: rhi::Mesh {
	Mesh(const fs::path& path);

	virtual void Draw(rhi::ShaderProgram& shader) override;

	winapi::ComPtr<ID3D12Resource> buffer;
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
	D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
};

}
