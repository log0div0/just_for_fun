
#pragma once

#include "../Vertex.hpp"

#include "../rhi/Mesh.hpp"
#include "ShaderProgram.hpp"

namespace opengl {

struct Mesh: rhi::Mesh {
	static const inline GLuint binding_index = 1; // any vacant value

	Mesh(const fs::path& path);

	void BindPos(GLuint location);
	void BindUV(GLuint location);
	void BindNormal(GLuint location);

	virtual void Draw(rhi::ShaderProgram& shader_rhi) override;

	mogl::ArrayBuffer vertex_buffer;
	mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;
	uint32_t index_count = 0;
};

}
