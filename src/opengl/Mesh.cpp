#include "Mesh.hpp"
#include "Context.hpp"

namespace opengl {

Mesh::Mesh(const fs::path& path) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::tie(vertices, indices) = LoadFromFile(path);

	vertex_buffer.setData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

	BindPos(0);
	BindUV(1);
	BindNormal(2);

	index_buffer.setData(indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	vertex_array.setElementBuffer(index_buffer.getHandle());

	index_count = indices.size();

}

void Mesh::BindPos(GLuint location) {
	vertex_array.setAttribBinding(location, binding_index);
	vertex_array.setAttribFormat(location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	vertex_array.enableAttrib(location);
}

void Mesh::BindUV(GLuint location) {
	vertex_array.setAttribBinding(location, binding_index);
	vertex_array.setAttribFormat(location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	vertex_array.enableAttrib(location);
}

void Mesh::BindNormal(GLuint location) {
	vertex_array.setAttribBinding(location, binding_index);
	vertex_array.setAttribFormat(location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	vertex_array.enableAttrib(location);
}

void Mesh::Draw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	g_context->CommitAll();

	shader.shader_program.use();

	vertex_array.bind();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
}

}