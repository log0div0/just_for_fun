
#pragma once

#include "../Vertex.hpp"

#include "../rhi/BoxMesh.hpp"

#include "ShaderProgram.hpp"

namespace vulkan {

struct BoxMesh: rhi::BoxMesh {
	BoxMesh() {
	}

	virtual void DoDraw(rhi::ShaderProgram& shader_rhi) override {
	}
};

}
