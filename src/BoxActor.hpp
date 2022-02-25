
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <math/Transform.hpp>

#include "Camera.hpp"

struct BoxActor {
	BoxActor();

	void InitShaders();
	void InitTextures();
	void InitMesh();

	void Update(float delta_time);
	void Render(const Camera& camera);

	mogl::ShaderProgram shader_program;
	mogl::Texture wood_texture = GL_TEXTURE_2D;
	mogl::Texture lambda_texture = GL_TEXTURE_2D;
	mogl::ArrayBuffer vertex_buffer;
	mogl::ElementArrayBuffer index_buffer;
	mogl::VertexArray vertex_array;

	math::Transform transformation;
};
