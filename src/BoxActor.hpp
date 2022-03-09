
#pragma once

#include <math/Transform.hpp>

#include "Camera.hpp"
#include "PointLight.hpp"
#include "render/Texture2D.hpp"

struct BoxActor {
	BoxActor();

	void Update(float delta_time);
	void Render(const Camera& camera, const PointLight& light);

	render::ShaderProgram shader_program;
	render::Texture2D wood_texture;
	render::Texture2D lambda_texture;
	render::BoxMesh mesh;

	math::Vector3 color = {1.0f, 0.5f, 0.31f};
};
