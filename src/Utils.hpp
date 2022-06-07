
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

fs::path GetAssetsDir();
std::vector<uint8_t> LoadBinaryFile(const fs::path& path);
std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
