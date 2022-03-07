#include "OpenGL.hpp"
#include "../Vertex.hpp"
#include <stb/Image.hpp>

namespace rhi {

Context::Context(glfw::Window& window_): window(window_) {
	glfw::makeContextCurrent(window);
	glfw::swapInterval(0);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	window.framebufferSizeEvent.subscribe([&](glfw::Window& window, int w, int h) {
		glViewport(0, 0, w, h);
	});
	auto [w, h] = window.getFramebufferSize();
	glViewport(0, 0, w, h);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_GREATER);
	glClearDepth(0.0);
}

void Context::Clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Context::Present() {
	window.swapBuffers();
}

void Context::WaitIdle() {

}

ShaderProgram::ShaderProgram(const fs::path& vertex, const fs::path& fragment)
{
	mogl::Shader vertex_shader(GL_VERTEX_SHADER);
	mogl::Shader fragment_shader(GL_FRAGMENT_SHADER);

	vertex_shader.compile(LoadTextFile(vertex));
	fragment_shader.compile(LoadTextFile(fragment));

	shader_program.attach(vertex_shader);
	shader_program.attach(fragment_shader);
	shader_program.link();
}

void ShaderProgram::SetUniform(const std::string& name, float value) {
	shader_program.setUniform(name, value);
}

void ShaderProgram::SetUniform(const std::string& name, int value) {
	shader_program.setUniform(name, value);
}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector3& value) {
	shader_program.setUniformPtr<3, float>(name, (float*)&value);
}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector4& value) {
	shader_program.setUniformPtr<4, float>(name, (float*)&value);
}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix3& value) {
	shader_program.setUniformMatrixPtr<3, float>(name, (float*)&value);
}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix4& value) {
	shader_program.setUniformMatrixPtr<4, float>(name, (float*)&value);
}

void ShaderProgram::Use() {
	shader_program.use();
}


Texture2D::Texture2D(const fs::path& path)
{
	stb::Image img(path);
	texture.setStorage(5, GL_RGBA8, img.w, img.h);
	texture.setSubImage(0, 0, 0, img.w, img.h, img.c == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, img.data);
	texture.generateMipmap();
}

void Texture2D::Bind(uint32_t unit)
{
	texture.bind(unit);
}

BoxMesh::BoxMesh() {
	vertex_buffer.setData(box_vertices.size() * sizeof(Vertex), box_vertices.data(), GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

	BindPos(0);
	BindUV(1);
	BindNormal(2);

	index_buffer.setData(box_indices.size() * sizeof(uint32_t), box_indices.data(), GL_STATIC_DRAW);
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

}
