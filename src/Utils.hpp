
#pragma once

#include <scope_guard.hpp>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>

GLuint LoadShader(const GLenum type, const fs::path& path);
GLuint LoadShaderProgram(const fs::path& vertex_path, const fs::path& fragment_path);
std::string LoadTextFile(const fs::path& path);
