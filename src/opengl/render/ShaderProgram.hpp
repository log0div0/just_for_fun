#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <glad/glad.h>
#include <mogl/mogl.hpp>

namespace render {

struct ShaderProgram {
	ShaderProgram() = default;

	ShaderProgram(const std::string& name)
	{
		mogl::Shader vertex_shader(GL_VERTEX_SHADER);
		mogl::Shader fragment_shader(GL_FRAGMENT_SHADER);

		vertex_shader.compile(LoadTextFile(g_assets_dir / "shaders" / "glsl" / (name + ".vert")));
		fragment_shader.compile(LoadTextFile(g_assets_dir / "shaders" / "glsl" / (name + ".frag")));

		shader_program.attach(vertex_shader);
		shader_program.attach(fragment_shader);
		shader_program.link();
	}

	void SetUniform(const std::string& name, float value) {
		shader_program.setUniform(name, value);
	}

	void SetUniform(const std::string& name, int value) {
		shader_program.setUniform(name, value);
	}

	void SetUniform(const std::string& name, const math::Vector3& value) {
		shader_program.setUniformPtr<3, float>(name, (float*)&value);
	}

	void SetUniform(const std::string& name, const math::Vector4& value) {
		shader_program.setUniformPtr<4, float>(name, (float*)&value);
	}

	void SetUniform(const std::string& name, const math::Matrix3& value) {
		shader_program.setUniformMatrixPtr<3, float>(name, (float*)&value);
	}

	void SetUniform(const std::string& name, const math::Matrix4& value) {
		shader_program.setUniformMatrixPtr<4, float>(name, (float*)&value);
	}

	void Use() {
		shader_program.use();
	}

	mogl::ShaderProgram shader_program;
};

}
