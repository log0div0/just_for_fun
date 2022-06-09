
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

fs::path GetAssetsDir();
std::vector<uint8_t> LoadBinaryFile(const fs::path& path);
std::string LoadTextFile(const fs::path& path);
void SaveBinaryFile(const fs::path& path, const uint8_t* data, size_t size);
float GetTimeSeconds();
