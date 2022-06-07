
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

extern fs::path g_assets_dir;

std::vector<uint8_t> LoadBinaryFile(const fs::path& path);
std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
