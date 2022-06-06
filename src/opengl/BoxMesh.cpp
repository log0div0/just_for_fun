#include "BoxMesh.hpp"
#include "Context.hpp"

namespace opengl {

BoxMesh::BoxMesh() {
	vertex_buffer.setData(box_vertices.size() * sizeof(Vertex), box_vertices.data(), GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

	BindPos(0);
	BindUV(1);
	BindNormal(2);

	// index_buffer.setData(box_indices.size() * sizeof(uint32_t), box_indices.data(), GL_STATIC_DRAW);
	// vertex_array.setElementBuffer(index_buffer.getHandle());

}

void BoxMesh::BindPos(GLuint location) {
	vertex_array.setAttribBinding(location, binding_index);
	vertex_array.setAttribFormat(location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	vertex_array.enableAttrib(location);
}

void BoxMesh::BindUV(GLuint location) {
	vertex_array.setAttribBinding(location, binding_index);
	vertex_array.setAttribFormat(location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	vertex_array.enableAttrib(location);
}

void BoxMesh::BindNormal(GLuint location) {
	vertex_array.setAttribBinding(location, binding_index);
	vertex_array.setAttribFormat(location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	vertex_array.enableAttrib(location);
}

void BoxMesh::DoDraw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	shader.shader_program.use();

	vertex_array.bind();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glDrawElements(GL_TRIANGLES, box_indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, box_vertices.size());
}

}