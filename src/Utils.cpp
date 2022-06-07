
#include "Utils.hpp"
#include <iostream>
#include <scope_guard.hpp>

std::vector<uint8_t> LoadBinaryFile(const fs::path& path) {
	if (!fs::exists(path)) {
		throw std::runtime_error("file " + path.string() + " does not exist");
	}

	auto file_size = fs::file_size(path);
	std::ifstream f(path, std::ios::binary);
	if (!f) {
		throw std::runtime_error("failed to open file " + path.string());
	}
	std::vector<uint8_t> buf;
	buf.resize(file_size);
	f.read((char*)&buf[0], file_size);
	return buf;
}

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

std::chrono::high_resolution_clock::time_point program_start = std::chrono::high_resolution_clock::now();

float GetTimeSeconds() {
	std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
	auto duration = now - program_start;
	return float(std::chrono::duration_cast<std::chrono::microseconds>(duration).count()) / 1000000;
}
