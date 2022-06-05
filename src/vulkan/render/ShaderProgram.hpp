#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include "Texture2D.hpp"

#include <vulkan/vulkan_raii.hpp>

namespace render {

struct ShaderProgram {
	ShaderProgram() = default;

	ShaderProgram(const std::string& name);

	void SetParam(const std::string& name, float value) {
	}

	void SetParam(const std::string& name, int value) {
	}

	void SetParam(const std::string& name, const math::Vector3& value) {
	}

	void SetParam(const std::string& name, const math::Vector4& value) {
	}

	void SetParam(const std::string& name, const math::Matrix3& value) {
	}

	void SetParam(const std::string& name, const math::Matrix4& value) {
	}

	void SetParam(const std::string& name, Texture2D& value) {
	}

	vk::raii::ShaderModule vertex_shader = nullptr;
	vk::raii::ShaderModule fragment_shader = nullptr;
};

}
