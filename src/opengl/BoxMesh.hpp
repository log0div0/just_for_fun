
#pragma once

#include "../Vertex.hpp"

#include "../rhi/BoxMesh.hpp"
#include "ShaderProgram.hpp"

namespace opengl {

struct BoxMesh: rhi::BoxMesh {
	static const inline GLuint binding_index = 1; // any vacant value

	BoxMesh();

	void BindPos(GLuint location);
	void BindUV(GLuint location);
	void BindNormal(GLuint location);

	virtual void Draw(rhi::ShaderProgram& shader_rhi) override;

	mogl::ArrayBuffer vertex_buffer;
	// mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;
};

}
