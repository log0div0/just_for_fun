
#pragma once

#include <math/Transform.hpp>

#include "Camera.hpp"
#include "PointLight.hpp"
#include "rhi/Texture2D.hpp"

struct BoxActor {
	BoxActor();

	void Update(float delta_time);
	void Render(const Camera& camera, const PointLight& light);

	std::unique_ptr<rhi::ShaderProgram> shader_program;
	std::unique_ptr<rhi::Texture2D> wood_texture;
	std::unique_ptr<rhi::Texture2D> lambda_texture;
	std::unique_ptr<rhi::Mesh> mesh;

	math::Vector3 color = {1.0f, 0.5f, 0.31f};
};
