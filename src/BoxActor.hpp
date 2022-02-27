
#pragma once

#include <math/Transform.hpp>

#include "Camera.hpp"
#include "PointLight.hpp"
#include "BoxMesh.hpp"

struct BoxActor {
	BoxActor();

	void Update(float delta_time);
	void Render(const Camera& camera, const PointLight& light);

	mogl::ShaderProgram shader_program;
	mogl::Texture2D wood_texture;
	mogl::Texture2D lambda_texture;
	BoxMesh mesh;
};
