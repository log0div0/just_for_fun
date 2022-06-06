
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <array>

enum {
	UNIFORM_BUFFERS_COUNT = 8,
	UNIFORM_BUFFER_SIZE = 1024
};

namespace rhi
{

struct Texture2D;
struct ShaderProgram;
struct BoxMesh;

struct UniformBuffer {
	UniformBuffer() = default;
	UniformBuffer(size_t size);
	~UniformBuffer();

	UniformBuffer(const UniformBuffer& other) = delete;
	UniformBuffer& operator=(const UniformBuffer& other) = delete;

	UniformBuffer(UniformBuffer&& other);
	UniformBuffer& operator=(UniformBuffer&& other);

	void Write(size_t offset, const uint8_t* src, int num_rows, int src_stride, int dst_stride);
	void Reset();

	const uint8_t* GetData() const;
	size_t GetSize() const;

private:
	uint8_t* shadow_data = nullptr;
	size_t max_size = 0;
	size_t current_size = 0;
};

struct Context {
	Context();
	virtual ~Context() {}

	virtual Texture2D* CreateTexture2D(const fs::path& path) = 0;
	virtual ShaderProgram* CreateShaderProgram(const std::string& name) = 0;
	virtual BoxMesh* CreateBoxMesh() = 0;

	virtual void Clear() = 0;
	virtual void Present() = 0;
	virtual void WaitIdle() = 0;

	virtual void ImGuiInit() = 0;
	virtual void ImGuiShutdown() = 0;
	virtual void ImGuiNewFrame() = 0;
	virtual void ImGuiRender() = 0;

	virtual void CommitResources() = 0;

	std::array<UniformBuffer, UNIFORM_BUFFERS_COUNT> uniform_buffers = {};
};

extern Context* g_context;

}
