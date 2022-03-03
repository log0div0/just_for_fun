
#pragma once

#include <math/Transform.hpp>

#include "Camera.hpp"
#include "PointLight.hpp"

struct BoxActor {
	BoxActor();

	void Update(float delta_time);
	void Render(const Camera& camera, const PointLight& light);

	rhi::ShaderProgram shader_program;
	rhi::Texture2D wood_texture;
	rhi::Texture2D lambda_texture;
	rhi::BoxMesh mesh;

	math::Vector3 color = {1.0f, 0.5f, 0.31f};
};
