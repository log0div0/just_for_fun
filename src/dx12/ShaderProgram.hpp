
#pragma once

#include "../rhi/ShaderProgram.hpp"

#include "Texture2D.hpp"

#include "details/Frame.hpp"
#include "details/DescriptorHeap.hpp"
#include "details/ConstantBuffer.hpp"

namespace dx12 {

struct ShaderProgram: rhi::ShaderProgram {
	ShaderProgram(const fs::path& path);

	virtual void SetParam(const std::string& name, rhi::Texture2D& value) override;

	winapi::ComPtr<ID3D12PipelineState> pipeline_state;

private:
	void PopulateBindingsMap(winapi::ComPtr<IDxcBlob> shader_blob);

	std::map<std::string, int> texture_bindings;
};

}
