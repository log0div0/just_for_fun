
#pragma once

#include "Camera.hpp"
#include "BoxMesh.hpp"

struct PointLight {
	PointLight();

	void Update(float delta_time);
	void Render(const Camera& camera);

	mogl::ShaderProgram shader_program;
	BoxMesh mesh;

	math::Vector3 color = {1.0f, 1.0f, 1.0f};
	math::Vector3 pos;
};
