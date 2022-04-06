
#pragma once

#include "Texture2D.hpp"
#include "Frame.hpp"

#include "DescriptorHeap.hpp"

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <winapi/ComPtr.hpp>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <map>
#include <array>

namespace render {

struct ParamInfo {
	UINT root_parameter_index;
	UINT offset;
	D3D12_ROOT_PARAMETER_TYPE type;
	std::array<winapi::ComPtr<ID3D12Resource>, NUM_FRAMES_IN_FLIGHT> constant_buffers;
	std::array<DescriptorPair, NUM_FRAMES_IN_FLIGHT> descriptor_tables;
};

struct ShaderProgram {
	ShaderProgram() = default;
	ShaderProgram(const std::string& name);
	void Use();
	void SetParam(const std::string& name, const math::Vector3& value);
	void SetParam(const std::string& name, const math::Vector4& value);
	void SetParam(const std::string& name, const math::Matrix3& value);
	void SetParam(const std::string& name, const math::Matrix4& value);
	void SetParam(const std::string& name, Texture2D& value);
	void Done();

	winapi::ComPtr<ID3D12RootSignature> root_signature;
	winapi::ComPtr<ID3D12PipelineState> pipeline_state;

private:
	void SetParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride);
	void InitParamsInfo(const D3D12_ROOT_SIGNATURE_DESC1* root_sig_desc, winapi::ComPtr<ID3DBlob> shader_blob);
	ParamInfo GetParamInfo(const std::string& name) const;
	std::map<std::string, ParamInfo> params_map;
};

}
