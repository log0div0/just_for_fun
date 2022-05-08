
#include "PointLight.hpp"

PointLight::PointLight() {
	shader_program = render::ShaderProgram("point_light");
}

void PointLight::Update(float delta_time) {
	pos = math::Vector3({
		std::sin(GetTimeSeconds() / 2) * 1.0f,
		std::cos(GetTimeSeconds() / 2) * 1.0f,
		1.0f});
}

void PointLight::Render(const Camera& camera) {
	math::Transform view = camera.GetViewTransform();
	math::Transform projection = camera.GetProjectionTransform();
	math::Transform model = { pos, {}, {0.2f} };
	math::Transform MVP = projection * view * model;

	shader_program.SetParam("MVP", MVP);
	shader_program.SetParam("LightColor", color);

	mesh.Draw(shader_program);
}
