
#include "PointLight.hpp"

PointLight::PointLight() {
	shader_program = LoadShaders(g_assets_dir/"shaders"/"point_light.vert", g_assets_dir/"shaders"/"point_light.frag");
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

	shader_program.setUniformMatrixPtr<4, float>("model", (float*)&model);
	shader_program.setUniformMatrixPtr<4, float>("view", (float*)&view);
	shader_program.setUniformMatrixPtr<4, float>("projection", (float*)&projection);
	shader_program.setUniformPtr<3, float>("LightColor", (float*)&color);

	shader_program.use();

	mesh.Draw();
}
