#include "BoxMesh.hpp"
#include "Vertex.hpp"

std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}, {  0.0f,  0.0f, -1.0f,} },
	{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}, {  0.0f,  0.0f, -1.0f, } },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, {  0.0f,  0.0f, -1.0f, } },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, {  0.0f,  0.0f, -1.0f, } },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}, {  0.0f,  0.0f, -1.0f, } },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}, {  0.0f,  0.0f, -1.0f, } },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}, {  0.0f,  0.0f, 1.0f,} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}, {  0.0f,  0.0f, 1.0f,} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}, {  0.0f,  0.0f, 1.0f,} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}, {  0.0f,  0.0f, 1.0f,} },
	{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}, {  0.0f,  0.0f, 1.0f,} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}, {  0.0f,  0.0f, 1.0f,} },
	{ {-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, { -1.0f,  0.0f,  0.0f,} },
	{ {-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, { -1.0f,  0.0f,  0.0f,} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, { -1.0f,  0.0f,  0.0f,} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, { -1.0f,  0.0f,  0.0f,} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}, { -1.0f,  0.0f,  0.0f,} },
	{ {-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, { -1.0f,  0.0f,  0.0f,} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, {  1.0f,  0.0f,  0.0f,} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, {  1.0f,  0.0f,  0.0f,} },
	{ { 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, {  1.0f,  0.0f,  0.0f,} },
	{ { 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, {  1.0f,  0.0f,  0.0f,} },
	{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}, {  1.0f,  0.0f,  0.0f,} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, {  1.0f,  0.0f,  0.0f,} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, {  0.0f, -1.0f,  0.0f,} },
	{ { 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f}, {  0.0f, -1.0f,  0.0f,} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}, {  0.0f, -1.0f,  0.0f,} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}, {  0.0f, -1.0f,  0.0f,} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}, {  0.0f, -1.0f,  0.0f,} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}, {  0.0f, -1.0f,  0.0f,} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}, {  0.0f,  1.0f,  0.0f,} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}, {  0.0f,  1.0f,  0.0f,} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, {  0.0f,  1.0f,  0.0f,} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}, {  0.0f,  1.0f,  0.0f,} },
	{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f}, {  0.0f,  1.0f,  0.0f,} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}, {  0.0f,  1.0f,  0.0f,} },
};

std::vector<uint32_t> indices = {
	0, 1, 3,
	1, 2, 3
};

BoxMesh::BoxMesh() {
	vertex_buffer.setData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

	BindPos(0);
	BindUV(1);
	BindNormal(2);

	index_buffer.setData(indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	vertex_array.setElementBuffer(index_buffer.getHandle());

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

void BoxMesh::Draw() {
	vertex_array.bind();

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
