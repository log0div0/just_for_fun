
#pragma once

#include "../rhi/ShaderProgram.hpp"

#include "Texture2D.hpp"

#include "details/Frame.hpp"
#include "details/DescriptorHeap.hpp"
#include "details/ConstantBuffer.hpp"

#include <d3dcompiler.h>
#include <map>
#include <variant>

namespace dx12 {

struct ConstantBufferBindingInfo {
	UINT root_parameter_index;
	UINT offset;
	UINT size;
};

struct TextureBindingInfo {
	UINT root_parameter_index;
};

using BindingInfo = std::variant<ConstantBufferBindingInfo, TextureBindingInfo>;

struct ShaderProgram: rhi::ShaderProgram {
	ShaderProgram(const std::string& name);

	virtual void SetParam(const std::string& name, float value) override;
	virtual void SetParam(const std::string& name, int value) override;
	virtual void SetParam(const std::string& name, const math::Vector3& value) override;
	virtual void SetParam(const std::string& name, const math::Vector4& value) override;
	virtual void SetParam(const std::string& name, const math::Matrix3& value) override;
	virtual void SetParam(const std::string& name, const math::Matrix4& value) override;
	virtual void SetParam(const std::string& name, rhi::Texture2D& value) override;

	winapi::ComPtr<ID3D12PipelineState> pipeline_state;

private:
	void PopulateBindingsMap(winapi::ComPtr<ID3DBlob> shader_blob);
	const BindingInfo& GetBindingInfo(const std::string& name) const;
	std::map<std::string, BindingInfo> bindings_map;

	void SetConstantParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride);
};

}
