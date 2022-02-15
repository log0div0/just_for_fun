
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>

uint32_t LoadShader(const GLenum type, const fs::path& path);
std::string LoadTextFile(const fs::path& path);
