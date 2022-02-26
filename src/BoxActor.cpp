
#include "BoxActor.hpp"
#include "Utils.hpp"
#include "Vertex.hpp"

#include <stb/Image.hpp>

extern fs::path g_assets_dir;

BoxActor::BoxActor() {
	InitShaders();
	InitTextures();
	InitMesh();
}

void BoxActor::InitShaders() {
	mogl::Shader vertex_shader(GL_VERTEX_SHADER);
	mogl::Shader fragment_shader(GL_FRAGMENT_SHADER);

	vertex_shader.compile(LoadTextFile(g_assets_dir / "shaders" / "shader.vert"));
	fragment_shader.compile(LoadTextFile(g_assets_dir / "shaders" / "shader.frag"));

	shader_program.attach(vertex_shader);
	shader_program.attach(fragment_shader);
	shader_program.link();

	shader_program.setUniform("Wood", 3);
	shader_program.setUniform("Lambda", 1);
}

void BoxActor::InitTextures() {
	{
		stb::Image img(g_assets_dir / "textures" / "Wood_Crate_001_basecolor.jpg", 3);
		wood_texture.setStorage2D(5, GL_RGBA8, img.w, img.h);
		wood_texture.setSubImage2D(0, 0, 0, img.w, img.h, GL_RGB, GL_UNSIGNED_BYTE, img.data);
		wood_texture.generateMipmap();
	}

	{
		stb::Image img(g_assets_dir / "textures" / "Half-Life_lambda_logo.png", 4);
		lambda_texture.setStorage2D(5, GL_RGBA8, img.w, img.h);
		lambda_texture.setSubImage2D(0, 0, 0, img.w, img.h, GL_RGBA, GL_UNSIGNED_BYTE, img.data);
		lambda_texture.generateMipmap();
	}
}

std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f, -0.5f},  {0.0f, 0.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f} },
	{ { 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ { 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f} },
	{ {-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f} },
	{ {-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f} }
};

std::vector<uint32_t> indices = {
	0, 1, 3,
	1, 2, 3
};

void BoxActor::InitMesh() {

	GLuint pos = shader_program.getAttribLocation("aPos");
	// GLuint color = shader_program.getAttribLocation("aColor");
	GLuint uv = shader_program.getAttribLocation("aUV");
	GLuint binding_index = 1; // any vacant value

	vertex_buffer.setData(vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
	vertex_array.setVertexBuffer(binding_index, vertex_buffer.getHandle(), 0, sizeof(Vertex));

	vertex_array.setAttribBinding(pos, binding_index);
	vertex_array.setAttribFormat(pos, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, pos));
	vertex_array.enableAttrib(pos);

	// vertex_array.setAttribBinding(color, binding_index);
	// vertex_array.setAttribFormat(color, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, color));
	// vertex_array.enableAttrib(color);

	vertex_array.setAttribBinding(uv, binding_index);
	vertex_array.setAttribFormat(uv, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	vertex_array.enableAttrib(uv);

	index_buffer.setData(indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
	vertex_array.setElementBuffer(index_buffer.getHandle());
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
	vertex_array.bind();
	wood_texture.bind(3);
	lambda_texture.bind(1);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
