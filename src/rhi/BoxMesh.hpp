
#pragma once

#include "ShaderProgram.hpp"

namespace rhi
{

struct BoxMesh {
	virtual ~BoxMesh() = 0 {}

	void Draw(ShaderProgram& shader_program) {
		g_context->CommitResources();
		DoDraw(shader_program);
	}

	virtual void DoDraw(ShaderProgram& shader_program) = 0;
};

}
