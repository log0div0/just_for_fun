
#pragma once

#include "../../Vertex.hpp"

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include "ShaderProgram.hpp"

namespace render {

struct BoxMesh {
	static const inline GLuint binding_index = 1; // any vacant value

	BoxMesh() {
		vertex_buffer.setData(box_vertices.size() * sizeof(Vertex), box_vertices.data(), GL_STATIC_DRAW);
		vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

		BindPos(0);
		BindUV(1);
		BindNormal(2);

		// index_buffer.setData(box_indices.size() * sizeof(uint32_t), box_indices.data(), GL_STATIC_DRAW);
		// vertex_array.setElementBuffer(index_buffer.getHandle());

	}

	void BindPos(GLuint location) {
		vertex_array.setAttribBinding(location, binding_index);
		vertex_array.setAttribFormat(location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
		vertex_array.enableAttrib(location);
	}

	void BindUV(GLuint location) {
		vertex_array.setAttribBinding(location, binding_index);
		vertex_array.setAttribFormat(location, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
		vertex_array.enableAttrib(location);
	}

	void BindNormal(GLuint location) {
		vertex_array.setAttribBinding(location, binding_index);
		vertex_array.setAttribFormat(location, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
		vertex_array.enableAttrib(location);
	}

	void Draw(ShaderProgram& shader) {
		shader.shader_program.use();

		vertex_array.bind();

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// glDrawElements(GL_TRIANGLES, box_indices.size(), GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, box_vertices.size());
	}

	mogl::ArrayBuffer vertex_buffer;
	// mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;
};

}
