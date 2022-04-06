
#pragma once

#include <string>
#include <algorithm>
#include <stdexcept>
#include <stb_image.h>
#include <stb_image_write.h>
#include <stb_image_resize.h>
#include <string.h>

namespace stb {

struct Image {
	Image() {}

	Image(int w_, int h_, int c_) {
		w = w_;
		h = h_;
		c = c_;
		data = (uint8_t*)malloc(w*h*c);
		if (!data) {
			throw std::runtime_error("malloc failed");
		}
	}

	Image(const char* path, int desired_channels = 0) {
		data = stbi_load(path, &w, &h, &c, desired_channels);
		if (desired_channels) {
			c = desired_channels;
		}
		if (!data) {
			throw std::runtime_error("Cannot load image " + std::string(path) + " because " + stbi_failure_reason());
		}
	}

	Image(const std::string& path, int desired_channels = 0): Image(path.c_str(), desired_channels) {}

	Image(const uint8_t* buffer, size_t buffer_len, int desired_channels = 0) {
		data = stbi_load_from_memory(buffer, buffer_len, &w, &h, &c, desired_channels);
		if (!data) {
			throw std::runtime_error(std::string("Cannot parse image because ") + stbi_failure_reason());
		}
	}

	~Image() {
		if (data) {
			stbi_image_free(data);
			data = nullptr;
		}
	}

	Image(const Image& other): Image(other.w, other.h, other.c) {
		memcpy(data, other.data, other.data_len());
	}

	Image& operator=(const Image& other) {
		uint8_t* new_data = (uint8_t*)realloc(data, other.data_len());
		if (!new_data) {
			throw std::runtime_error("realloc failed");
		}
		data = new_data;
		w = other.w;
		h = other.h;
		c = other.c;
		memcpy(data, other.data, other.data_len());
		return *this;
	}

	Image(Image&& other):
		data(other.data),
		w(other.w),
		h(other.h),
		c(other.c)
	{
		other.data = nullptr;
		other.w = 0;
		other.h = 0;
		other.c = 0;
	}

	Image& operator=(Image&& other) {
		std::swap(data, other.data);
		std::swap(w, other.w);
		std::swap(h, other.h);
		std::swap(c, other.c);
		return *this;
	}

	void write_png(const std::string& path) const {
		if (!stbi_write_png(path.c_str(), w, h, c, data, w*c)) {
			throw std::runtime_error("Cannot save image " + path + " because " + stbi_failure_reason());
		}
	}

	void write_jpg(const std::string& path, int quality) const {
		if (!stbi_write_jpg(path.c_str(), w, h, c, data, quality)) {
			throw std::runtime_error("Cannot save image " + path + " because " + stbi_failure_reason());
		}
	}

	size_t pixels_count() const {
		return w * h;
	}

	size_t data_len() const {
		return w * h * c;
	}

	size_t stride() const {
		return w * c;
	}

	Image resize(int out_w, int out_h) const {
		Image result(out_w, out_h, c);
		if (!stbir_resize_uint8(
			this->data, this->w, this->h, this->stride(),
			result.data, result.w, result.h, result.stride(),
			c)
		) {
			throw std::runtime_error("stbir_resize_uint8 failed");
		}
		return result;
	}

	uint8_t* data = nullptr;
	int w = 0;
	int h = 0;
	int c = 0;
};

}
