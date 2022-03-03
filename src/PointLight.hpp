
#pragma once

#include "Camera.hpp"
#include "RHI.hpp"

struct PointLight {
	PointLight();

	void Update(float delta_time);
	void Render(const Camera& camera);

	rhi::ShaderProgram shader_program;
	rhi::BoxMesh mesh;

	math::Vector3 color = {1.0f, 1.0f, 1.0f};
	math::Vector3 pos;
};
