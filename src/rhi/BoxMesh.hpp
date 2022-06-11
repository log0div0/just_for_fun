
#pragma once

#include "ShaderProgram.hpp"

namespace rhi
{

struct BoxMesh {
	virtual ~BoxMesh() = 0 {}

	virtual void Draw(ShaderProgram& shader_program) = 0;
};

}
