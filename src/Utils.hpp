
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <glfwpp/glfwpp.h>

std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
mogl::ShaderProgram LoadShaders(const fs::path& vertex, const fs::path& fragment);
mogl::Texture2D LoadTexture(const fs::path& path);