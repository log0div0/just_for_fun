
#pragma once

#include "../Vertex.hpp"

#include "../rhi/Mesh.hpp"

#include "ShaderProgram.hpp"
#include "details/Buffer.hpp"

namespace vulkan {

struct Mesh: rhi::Mesh {
	Mesh(const fs::path& path);

	virtual void Draw(rhi::ShaderProgram& shader_rhi) override;

private:
	Buffer vertex_buffer, index_buffer;
};

}
