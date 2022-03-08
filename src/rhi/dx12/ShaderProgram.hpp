
#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

namespace rhi {

struct ShaderProgram {
	ShaderProgram() = default;
	ShaderProgram(const std::string& name);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, const math::Vector3& value);
	void SetUniform(const std::string& name, const math::Vector4& value);
	void SetUniform(const std::string& name, const math::Matrix3& value);
	void SetUniform(const std::string& name, const math::Matrix4& value);
	void Use();
};

}
