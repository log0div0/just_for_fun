
#pragma once

#include "Camera.hpp"
#include "render/ShaderProgram.hpp"
#include "render/BoxMesh.hpp"

struct PointLight {
	PointLight();

	void Update(float delta_time);
	void Render(const Camera& camera);

	render::ShaderProgram shader_program;
	render::BoxMesh mesh;

	math::Vector3 color = {1.0f, 1.0f, 1.0f};
	math::Vector3 pos;
};
