
#include "BoxActor.hpp"
#include "Utils.hpp"

BoxActor::BoxActor() {
	shader_program = render::ShaderProgram("box_actor");
	shader_program.SetUniform("Wood", 3);
	shader_program.SetUniform("Lambda", 1);

	wood_texture = render::Texture2D(g_assets_dir/"textures"/"Wood_Crate_001_basecolor.jpg");
	lambda_texture = render::Texture2D(g_assets_dir/"textures"/"Half-Life_lambda_logo.png");
}

void BoxActor::Update(float delta_time) {
}

void BoxActor::Render(const Camera& camera, const PointLight& light) {
	math::Transform view = camera.GetViewTransform();
	math::Transform projection = camera.GetProjectionTransform();
	math::Transform model = {};
	math::Matrix3 normal = model.Remove(3, 3).Adjugate();
	math::Transform MVP = projection * view * model;

	shader_program.Use();
	shader_program.SetUniform("MVP", MVP);
	shader_program.SetUniform("ModelMatrix", model);
	shader_program.SetUniform("NormalMatrix", normal);
	// shader_program.SetUniform("ObjectColor", color);
	shader_program.SetUniform("LightPos", light.pos);
	shader_program.SetUniform("LightColor", light.color);
	shader_program.SetUniform("CameraPos", camera.pos);

	wood_texture.Bind(3);
	lambda_texture.Bind(1);

	mesh.Draw();
}
