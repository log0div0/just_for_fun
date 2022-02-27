
#include "Utils.hpp"
#include <iostream>
#include <scope_guard.hpp>
#include <stb/Image.hpp>

std::string LoadTextFile(const fs::path& path) {
	if (!fs::exists(path)) {
		throw std::runtime_error("file " + path.string() + " does not exist");
	}

	auto file_size = fs::file_size(path);
	std::ifstream f(path, std::ios::binary);
	if (!f) {
		throw std::runtime_error("failed to open file " + path.string());
	}
	std::string str;
	str.resize(file_size);
	f.read(&str[0], file_size);
	return str;
}

std::chrono::high_resolution_clock::time_point program_start = std::chrono::high_resolution_clock::now();

float GetTimeSeconds() {
	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	auto duration = now - program_start;
	return float(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) / 1000000;
}

mogl::ShaderProgram LoadShaders(const fs::path& vertex, const fs::path& fragment)
{
	mogl::Shader vertex_shader(GL_VERTEX_SHADER);
	mogl::Shader fragment_shader(GL_FRAGMENT_SHADER);

	vertex_shader.compile(LoadTextFile(vertex));
	fragment_shader.compile(LoadTextFile(fragment));

	mogl::ShaderProgram shader_program;
	shader_program.attach(vertex_shader);
	shader_program.attach(fragment_shader);
	shader_program.link();
	return shader_program;
}

mogl::Texture2D LoadTexture(const fs::path& path)
{
	stb::Image img(path);
	mogl::Texture2D texture;
	texture.setStorage(5, GL_RGBA8, img.w, img.h);
	texture.setSubImage(0, 0, 0, img.w, img.h, img.c == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, img.data);
	texture.generateMipmap();
	return texture;
}
