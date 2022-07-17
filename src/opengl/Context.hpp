
#pragma once

#include "Mesh.hpp"
#include "ShaderProgram.hpp"
#include "Texture2D.hpp"

#include "../Window.hpp"
#include "../Utils.hpp"

namespace opengl {

struct Context: rhi::Context {
	Context(Window& window_);
	virtual ~Context() override;

	virtual rhi::Texture2D* CreateTexture2D(const fs::path& path) override { return new Texture2D(path); }
	virtual rhi::ShaderProgram* CreateShaderProgram(const fs::path& path) override { return new ShaderProgram(path); }
	virtual rhi::Mesh* CreateMesh(const fs::path& path) override { return new Mesh(path); }

	virtual void Clear() override;
	virtual void Present() override;
	virtual void WaitIdle() override;

	virtual void ImGuiInit() override;
	virtual void ImGuiShutdown() override;
	virtual void ImGuiNewFrame() override;
	virtual void ImGuiRender() override;

	void CommitAll();

	Window& window;

	std::vector<mogl::UniformBuffer> gl_uniform_buffers;
};

extern Context* g_context;

}
