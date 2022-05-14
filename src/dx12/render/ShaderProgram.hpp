
#pragma once

#include "Texture2D.hpp"

#include "details/Frame.hpp"
#include "details/DescriptorHeap.hpp"
#include "details/ConstantBuffer.hpp"

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <d3dcompiler.h>
#include <map>
#include <variant>

namespace render {

struct ConstantBufferBindingInfo {
	UINT root_parameter_index;
	UINT offset;
	UINT size;
};

struct TextureBindingInfo {
	UINT root_parameter_index;
};

using BindingInfo = std::variant<ConstantBufferBindingInfo, TextureBindingInfo>;

struct ShaderProgram {
	ShaderProgram() = default;
	ShaderProgram(const std::string& name);

	void SetParam(const std::string& name, const math::Vector3& value);
	void SetParam(const std::string& name, const math::Vector4& value);
	void SetParam(const std::string& name, const math::Matrix3& value);
	void SetParam(const std::string& name, const math::Matrix4& value);
	void SetParam(const std::string& name, Texture2D& value);

	winapi::ComPtr<ID3D12PipelineState> pipeline_state;

private:
	void PopulateBindingsMap(winapi::ComPtr<ID3DBlob> shader_blob);
	const BindingInfo& GetBindingInfo(const std::string& name) const;
	std::map<std::string, BindingInfo> bindings_map;

	void SetConstantParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride);
};

}
