
#pragma once

#include "../../Utils.hpp"

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

namespace rhi {

void SetViewportSize(int w, int h);
void ClearViewport();

struct ShaderProgram {
	ShaderProgram() = default;
	ShaderProgram(const fs::path& vertex, const fs::path& fragment);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, const math::Vector3& value);
	void SetUniform(const std::string& name, const math::Vector4& value);
	void SetUniform(const std::string& name, const math::Matrix3& value);
	void SetUniform(const std::string& name, const math::Matrix4& value);
	void Use();

	mogl::ShaderProgram shader_program;
};

struct Texture2D {
	Texture2D() = default;
	Texture2D(const fs::path& path);
	void Bind(uint32_t unit);

	mogl::Texture2D texture;
};

struct BoxMesh {
	static const inline GLuint binding_index = 1; // any vacant value

	BoxMesh();

	void BindPos(GLuint location);
	void BindUV(GLuint location);
	void BindNormal(GLuint location);

	void Draw();

	mogl::ArrayBuffer vertex_buffer;
	mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;
};

}
