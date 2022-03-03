#include "DX12.hpp"

namespace rhi {

Context::Context(glfw::Window& window_): window(window_) {
	throw std::runtime_error("TODO: init");
}

void Context::FrameBegin() {
	throw std::runtime_error("TODO: FrameBegin");
}

void Context::FrameEnd() {
	throw std::runtime_error("TODO: FrameEnd");
}

ShaderProgram::ShaderProgram(const fs::path& vertex, const fs::path& fragment) {

}

void ShaderProgram::SetUniform(const std::string& name, float value) {

}

void ShaderProgram::SetUniform(const std::string& name, int value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector3& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector4& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix3& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix4& value) {

}

void ShaderProgram::Use() {

}

Texture2D::Texture2D(const fs::path& path) {

}

void Texture2D::Bind(uint32_t unit) {

}


void BoxMesh::Draw() {

}



}
