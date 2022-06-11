
#pragma once

#include "../Vertex.hpp"

#include "../rhi/BoxMesh.hpp"

#include "ShaderProgram.hpp"
#include "details/Buffer.hpp"

namespace vulkan {

struct BoxMesh: rhi::BoxMesh {
	BoxMesh();

	virtual void Draw(rhi::ShaderProgram& shader_rhi) override;

private:
	Buffer mesh_buffer;
};

}
