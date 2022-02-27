
#include "BoxActor.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

extern fs::path g_assets_dir;

BoxActor::BoxActor() {
	InitShaders();
	InitTextures();
	InitMesh();
}

void BoxActor::InitShaders() {
	shader_program = LoadShaders(g_assets_dir/"shaders"/"shader.vert", g_assets_dir/"shaders"/"shader.frag");
	shader_program.setUniform("Wood", 3);
	shader_program.setUniform("Lambda", 1);
}

void BoxActor::InitTextures() {
	wood_texture = LoadTexture(g_assets_dir/"textures"/"Wood_Crate_001_basecolor.jpg");
	lambda_texture = LoadTexture(g_assets_dir/"textures"/"Half-Life_lambda_logo.png");
}

void BoxActor::InitMesh() {
	mesh.BindPos(shader_program.getAttribLocation("aPos"));
	// mesh.BindColor(shader_program.getAttribLocation("aColor"));
	mesh.BindUV(shader_program.getAttribLocation("aUV"));
}

void BoxActor::Update(float delta_time) {
	transformation = math::Rotate(GetTimeSeconds() * 0.1f, {0.0f, 0.0f, 1.0f});
}

void BoxActor::Render(const Camera& camera, const PointLight& light) {
	math::Transform view = camera.GetViewTransform();
	math::Transform projection = camera.GetProjectionTransform();

	shader_program.setUniformMatrixPtr<4, float>("model", (float*)&transformation);
	shader_program.setUniformMatrixPtr<4, float>("view", (float*)&view);
	shader_program.setUniformMatrixPtr<4, float>("projection", (float*)&projection);

	shader_program.use();
	wood_texture.bind(3);
	lambda_texture.bind(1);

	mesh.Draw();
}
