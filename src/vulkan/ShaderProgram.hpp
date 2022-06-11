#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include "Texture2D.hpp"

#include <vulkan/vulkan_raii.hpp>

#include "../rhi/ShaderProgram.hpp"

namespace vulkan {

struct ShaderProgram: rhi::ShaderProgram {
	ShaderProgram(const std::string& name);

	virtual void SetParam(const std::string& name, rhi::Texture2D& value) override {
	}

	vk::Pipeline GetPipeline();

private:
	vk::raii::ShaderModule LoadShaderModule(const std::string& path);

	vk::raii::ShaderModule vertex_shader = nullptr;
	vk::raii::ShaderModule fragment_shader = nullptr;

	uint16_t w = 0;
	uint16_t h = 0;

	vk::raii::Pipeline pipeline = nullptr;
};

}
