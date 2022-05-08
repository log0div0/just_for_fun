
#include "BoxActor.hpp"
#include "Utils.hpp"

BoxActor::BoxActor() {
	shader_program = render::ShaderProgram("box_actor");

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

	shader_program.SetParam("MVP", MVP);
	shader_program.SetParam("ModelMatrix", model);
	shader_program.SetParam("NormalMatrix", normal);
	// shader_program.SetParam("ObjectColor", color);
	shader_program.SetParam("LightPos", light.pos);
	shader_program.SetParam("LightColor", light.color);
	shader_program.SetParam("CameraPos", camera.pos);
	shader_program.SetParam("Wood", wood_texture);
	shader_program.SetParam("Lambda", lambda_texture);

	mesh.Draw(shader_program);
}
