
#include "Utils.hpp"
#include <iostream>
#include <scope_guard.hpp>

std::string LoadTextFile(const fs::path& path) {
	if (!fs::exists(path)) {
		throw std::runtime_error("file " + path.string() + " does not exist");
	}

	auto file_size = fs::file_size(path);
	std::ifstream f(path, std::ios::binary);
	if (!f) {
		throw std::runtime_error("failed to open file " + path.string());
	}
	std::string str;
	str.resize(file_size);
	f.read(&str[0], file_size);
	return str;
}
