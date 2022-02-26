
#pragma once

#include "Utils.hpp"

struct BoxMesh {
	static const inline GLuint binding_index = 1; // any vacant value

	BoxMesh();

	void BindPos(GLuint location);
	void BindColor(GLuint location);
	void BindUV(GLuint location);

	void Draw();

	mogl::ArrayBuffer vertex_buffer;
	mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;
};
