
#pragma once

#include "Camera.hpp"
#include "rhi/ShaderProgram.hpp"
#include "rhi/Mesh.hpp"

struct PointLight {
	PointLight();

	void Update(float delta_time);
	void Render(const Camera& camera);

	std::unique_ptr<rhi::ShaderProgram> shader_program;
	std::unique_ptr<rhi::Mesh> mesh;

	math::Vector3 color = {1.0f, 1.0f, 1.0f};
	math::Vector3 pos;
};
