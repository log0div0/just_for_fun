
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

std::string LoadTextFile(const fs::path& path);
float GetTimeSeconds();
