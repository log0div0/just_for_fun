#include "Context.hpp"
#include <cassert>

void* aligned_alloc(size_t alignment, size_t size) {
#ifdef WIN32
	return _aligned_malloc(size, alignment);
#else
	return std::aligned_alloc(alignment, size);
#endif
}

void aligned_free(void* p) {
#ifdef WIN32
	_aligned_free(p);
#else
	std::free(p);
#endif
}

namespace rhi
{

UniformBuffer::UniformBuffer(size_t size): max_size(size) {
	assert(size % 256 == 0);
	shadow_data = (uint8_t*)aligned_alloc(16, max_size);
}

UniformBuffer::~UniformBuffer() {
	if (shadow_data) {
		aligned_free(shadow_data);
	}
}

UniformBuffer& UniformBuffer::operator=(UniformBuffer&& other) {
	std::swap(shadow_data, other.shadow_data);
	std::swap(max_size, other.max_size);
	std::swap(current_size, other.current_size);
	return *this;
}

void UniformBuffer::Write(size_t offset, const uint8_t* src, int num_rows, int src_stride, int dst_stride) {
	size_t end_of_write = offset + num_rows*dst_stride;
	if (end_of_write > max_size) {
		throw std::runtime_error("Out of bounds while writing to the constant buffer");
	}
	uint8_t* dst = shadow_data + offset;
	for (int i = 0; i < num_rows; ++i) {
		memcpy(dst + i*dst_stride, src + i*src_stride, src_stride);
	}
	current_size = std::max(current_size, end_of_write);
}

void UniformBuffer::Reset() {
	current_size = 0;
}

size_t RoundUp(size_t n, size_t round_to) {
	return ((n + round_to-1) / round_to) * round_to;
}

const uint8_t* UniformBuffer::GetData() const {
	return shadow_data;
}

size_t UniformBuffer::GetSize() const {
	return RoundUp(current_size, 256);
}

Context::Context() {
	for (int i = 0; i < uniform_buffers.size(); ++i) {
		uniform_buffers[i] = UniformBuffer(UNIFORM_BUFFER_SIZE);
	}
}

}