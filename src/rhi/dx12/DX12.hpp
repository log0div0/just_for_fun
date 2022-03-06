#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include "Context.hpp"

namespace rhi {

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
};

struct Texture2D {
	Texture2D() = default;
	Texture2D(const fs::path& path);
	void Bind(uint32_t unit);
};

struct BoxMesh {
	void Draw();
};

}
