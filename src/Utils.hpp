
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glad/glad.h>
#include <mogl/mogl.hpp>

#include <glfwpp/glfwpp.h>

std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
