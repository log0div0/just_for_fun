
#pragma once

#include <scope_guard.hpp>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>

template <typename Fn>
void WithVertexArray(GLuint array, Fn&& fn) {
	glBindVertexArray(array);
	fn();
	glBindVertexArray(0);
}

template <typename Fn>
void WithBuffer(GLenum target, GLuint buffer, Fn&& fn) {
	glBindBuffer(target, buffer);
	fn();
	glBindBuffer(target, 0);
}

uint32_t LoadShader(const GLenum type, const fs::path& path);
uint32_t LoadShaderProgram(const fs::path& vertex_path, const fs::path& fragment_path);
std::string LoadTextFile(const fs::path& path);
