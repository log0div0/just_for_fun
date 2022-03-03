
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <glfwpp/glfwpp.h>

extern fs::path g_assets_dir;

std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
