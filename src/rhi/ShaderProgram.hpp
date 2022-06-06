
#pragma once

#include "Texture2D.hpp"

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <map>
#include <variant>

namespace rhi
{

struct UniformBinding {
	uint32_t buffer_index;
	uint32_t param_offset;
};

struct ShaderProgram {
	virtual ~ShaderProgram() {}

	void SetParam(const std::string& name, float value);
	void SetParam(const std::string& name, int value);
	void SetParam(const std::string& name, const math::Vector3& value);
	void SetParam(const std::string& name, const math::Vector4& value);
	void SetParam(const std::string& name, const math::Matrix3& value);
	void SetParam(const std::string& name, const math::Matrix4& value);

	virtual void SetParam(const std::string& name, Texture2D& value) = 0;

protected:
	UniformBinding GetUniformBinding(const std::string& name) const;
	std::map<std::string, UniformBinding> uniform_bindings;

	void SetUniformParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride);
};

}
