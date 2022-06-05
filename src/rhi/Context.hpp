
#pragma once

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

namespace rhi
{

struct Texture2D;
struct ShaderProgram;
struct BoxMesh;

struct Context {
	virtual ~Context() = 0 {}

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
};

extern Context* g_context;

}
