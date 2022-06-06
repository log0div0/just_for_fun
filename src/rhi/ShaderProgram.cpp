#include "ShaderProgram.hpp"

namespace rhi {

void ShaderProgram::SetParam(const std::string& name, float value) {
	SetUniformParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, int value) {
	SetUniformParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Vector3& value) {
	SetUniformParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Vector4& value) {
	SetUniformParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Matrix3& value) {
	SetUniformParam(name, (uint8_t*)&value, 3, sizeof(math::Vector3), sizeof(math::Vector4));
}

void ShaderProgram::SetParam(const std::string& name, const math::Matrix4& value) {
	SetUniformParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

UniformBinding ShaderProgram::GetUniformBinding(const std::string& name) const {
	auto it = uniform_bindings.find(name);
	if (it == uniform_bindings.end()) {
		throw std::runtime_error("Shader doesn't have param " + name);
	}
	return it->second;
}

void ShaderProgram::SetUniformParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride) {
	UniformBinding binding = GetUniformBinding(name);
	rhi::UniformBuffer& uniform_buffer = g_context->uniform_buffers.at(binding.buffer_index);
	uniform_buffer.Write(binding.param_offset, src, num_rows, src_stride, dst_stride);
}

}
