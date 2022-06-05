
#pragma once

#include "Texture2D.hpp"

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

namespace rhi
{

struct ShaderProgram {
	virtual ~ShaderProgram() = 0 {}
	virtual void SetParam(const std::string& name, float value) = 0;
	virtual void SetParam(const std::string& name, int value) = 0;
	virtual void SetParam(const std::string& name, const math::Vector3& value) = 0;
	virtual void SetParam(const std::string& name, const math::Vector4& value) = 0;
	virtual void SetParam(const std::string& name, const math::Matrix3& value) = 0;
	virtual void SetParam(const std::string& name, const math::Matrix4& value) = 0;
	virtual void SetParam(const std::string& name, Texture2D& value) = 0;
};

}