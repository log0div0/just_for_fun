
#pragma once

#include "../Vertex.hpp"

#include "../rhi/BoxMesh.hpp"

#include "ShaderProgram.hpp"

namespace vulkan {

struct BoxMesh: rhi::BoxMesh {
	BoxMesh() {
	}

	virtual void Draw(rhi::ShaderProgram& shader) override {
	}
};

}
