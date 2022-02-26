
#pragma once

#include <math/Transform.hpp>

#include "Camera.hpp"
#include "PointLight.hpp"
#include "BoxMesh.hpp"

struct BoxActor {
	BoxActor();

	void InitShaders();
	void InitTextures();
	void InitMesh();

	void Update(float delta_time);
	void Render(const Camera& camera, const PointLight& light);

	mogl::ShaderProgram shader_program;
	mogl::Texture wood_texture = GL_TEXTURE_2D;
	mogl::Texture lambda_texture = GL_TEXTURE_2D;
	BoxMesh mesh;

	math::Transform transformation;
};
