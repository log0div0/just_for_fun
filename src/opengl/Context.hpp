
#pragma once

#include "BoxMesh.hpp"
#include "ShaderProgram.hpp"
#include "Texture2D.hpp"

#include "../Utils.hpp"

namespace opengl {

struct Context: rhi::Context {
	Context(glfw::Window& window_);

	virtual ~Context() override {}

	virtual rhi::Texture2D* CreateTexture2D(const fs::path& path) override { return new Texture2D(path); }
	virtual rhi::ShaderProgram* CreateShaderProgram(const std::string& name) override { return new ShaderProgram(name); }
	virtual rhi::BoxMesh* CreateBoxMesh() override { return new BoxMesh(); }

	virtual void Clear() override;
	virtual void Present() override;
	virtual void WaitIdle() override;

	virtual void ImGuiInit() override;
	virtual void ImGuiShutdown() override;
	virtual void ImGuiNewFrame() override;
	virtual void ImGuiRender() override;

	virtual void CommitResources() override;

	glfw::Window& window;

	std::vector<mogl::UniformBuffer> gl_uniform_buffers;
};

}
