
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

extern fs::path g_assets_dir;

std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
