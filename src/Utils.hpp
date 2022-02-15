
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>

uint32_t LoadShader(const GLenum type, const fs::path& path);
uint32_t LoadShaderProgram(const fs::path& vertex_path, const fs::path& fragment_path);
std::string LoadTextFile(const fs::path& path);
