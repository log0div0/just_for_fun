
#pragma once

#include "../rhi/ShaderProgram.hpp"

#include "Texture2D.hpp"

#include "details/Frame.hpp"
#include "details/DescriptorHeap.hpp"
#include "details/ConstantBuffer.hpp"

#include <d3dcompiler.h>

namespace dx12 {

struct ShaderProgram: rhi::ShaderProgram {
	ShaderProgram(const std::string& name);

	virtual void SetParam(const std::string& name, rhi::Texture2D& value) override;

	winapi::ComPtr<ID3D12PipelineState> pipeline_state;

private:
	void PopulateBindingsMap(winapi::ComPtr<ID3DBlob> shader_blob);

	std::map<std::string, int> texture_bindings;
};

}
