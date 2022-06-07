#include "ShaderProgram.hpp"

#include "../Utils.hpp"

namespace opengl {

ShaderProgram::ShaderProgram(const std::string& name)
{
	mogl::Shader vertex_shader(GL_VERTEX_SHADER);
	mogl::Shader fragment_shader(GL_FRAGMENT_SHADER);

	vertex_shader.compile(LoadTextFile(GetAssetsDir() / "shaders" / "glsl" / (name + ".vert")));
	fragment_shader.compile(LoadTextFile(GetAssetsDir() / "shaders" / "glsl" / (name + ".frag")));

	shader_program.attach(vertex_shader);
	shader_program.attach(fragment_shader);
	shader_program.link();

	PopulateUniformBindings();
}

void ShaderProgram::SetParam(const std::string& name, rhi::Texture2D& value_rhi) {
	auto& value = static_cast<Texture2D&>(value_rhi);
	int unit = GetTextureUnit(name);
	shader_program.setUniform(name, unit);
	value.texture.bind(unit);
}

int ShaderProgram::GetTextureUnit(const std::string& name) {
	auto [it, _] = units_map.emplace(name, (int)units_map.size());
	return it->second;
}

void ShaderProgram::PopulateUniformBindings() {
	GLint num_uniforms = 0;
	glGetProgramiv(shader_program.getHandle(), GL_ACTIVE_UNIFORMS, &num_uniforms);

	std::vector<GLuint> uniform_indexes;
	uniform_indexes.reserve(num_uniforms);
	for (GLint i = 0; i < num_uniforms; ++i) {
		uniform_indexes.push_back(i);
	}

	std::vector<GLint> block_indexes(num_uniforms, -1);
	std::vector<GLint> uniform_offsets(num_uniforms, -1);
	glGetActiveUniformsiv(shader_program.getHandle(), num_uniforms, uniform_indexes.data(), GL_UNIFORM_BLOCK_INDEX, block_indexes.data());
	glGetActiveUniformsiv(shader_program.getHandle(), num_uniforms, uniform_indexes.data(), GL_UNIFORM_OFFSET, uniform_offsets.data());

	const GLuint buf_size = 128;
	GLchar name_buf[buf_size];
	for (GLuint i: uniform_indexes)
	{
		if (block_indexes.at(i) == -1) {
			continue;
		}
		GLsizei written = 0;
		GLint size = 0;
		GLenum type = 0;
		glGetActiveUniform(shader_program.getHandle(), i, buf_size, &written, &size, &type, name_buf);
		std::string name(name_buf, name_buf+written);
		GLint block_binding = -1;
		glGetActiveUniformBlockiv(shader_program.getHandle(), block_indexes.at(i), GL_UNIFORM_BLOCK_BINDING, &block_binding);
		uniform_bindings.emplace(name, rhi::UniformBinding{(uint32_t)block_binding, (uint32_t)uniform_offsets.at(i)});
	}
}

}