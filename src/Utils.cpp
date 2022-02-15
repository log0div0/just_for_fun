
#include "Utils.hpp"
#include <iostream>
#include <scope_guard.hpp>

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

GLuint LoadShader(const GLenum type, const fs::path& path) {
	GLuint handler = glCreateShader(type);
	SCOPE_FAIL{ glDeleteShader(handler); };

	std::string text = LoadTextFile(path);
	const char* c_text = text.c_str();
	glShaderSource(handler, 1, &c_text, nullptr);
	glCompileShader(handler);

	int success = 0;
	glGetShaderiv(handler, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		int log_length = 0;
		glGetShaderiv(handler, GL_INFO_LOG_LENGTH, &log_length);
		std::string log;
		log.resize(log_length);
		glGetShaderInfoLog(handler, log_length, NULL, &log[0]);

		throw std::runtime_error("Failed to compile " + path.string() + ": " + log);
	}

	return handler;
}

GLuint LoadShaderProgram(const fs::path& vertex_path, const fs::path& fragment_path) {
	GLuint vertex_shader = LoadShader(GL_VERTEX_SHADER, vertex_path);
	SCOPE_EXIT{ glDeleteShader(vertex_shader); };
	GLuint fragment_shader = LoadShader(GL_FRAGMENT_SHADER, fragment_path);
	SCOPE_EXIT{ glDeleteShader(fragment_shader); };

	GLuint shader_program = glCreateProgram();
	SCOPE_FAIL{ glDeleteProgram(shader_program); };

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);
	glLinkProgram(shader_program);

	int success = 0;
	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

	if(!success)
	{
		int log_length = 0;
		glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &log_length);
		std::string log;
		log.resize(log_length);
		glGetProgramInfoLog(shader_program, log_length, NULL, &log[0]);

		throw std::runtime_error("Failed to link shaders " + vertex_path.string() + " and " + fragment_path.string() + ": " + log);
	}

	return shader_program;
}