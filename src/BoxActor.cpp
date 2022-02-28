
#include "BoxActor.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

BoxActor::BoxActor() {
	shader_program = LoadShaders(g_assets_dir/"shaders"/"box_actor.vert", g_assets_dir/"shaders"/"box_actor.frag");
	shader_program.setUniform("Wood", 3);
	shader_program.setUniform("Lambda", 1);

	wood_texture = LoadTexture(g_assets_dir/"textures"/"Wood_Crate_001_basecolor.jpg");
	lambda_texture = LoadTexture(g_assets_dir/"textures"/"Half-Life_lambda_logo.png");
}

void BoxActor::Update(float delta_time) {
}

void BoxActor::Render(const Camera& camera, const PointLight& light) {
	math::Transform view = camera.GetViewTransform();
	math::Transform projection = camera.GetProjectionTransform();
	math::Transform model = {};

	shader_program.setUniformMatrixPtr<4, float>("model", (float*)&model);
	shader_program.setUniformMatrixPtr<4, float>("view", (float*)&view);
	shader_program.setUniformMatrixPtr<4, float>("projection", (float*)&projection);
	shader_program.setUniformPtr<3, float>("ObjectColor", (float*)&color);

	shader_program.use();
	wood_texture.bind(3);
	lambda_texture.bind(1);

	mesh.Draw();
}
