
#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include <ghc/filesystem.hpp>
namespace fs = ghc::filesystem;

#include <winapi/ComPtr.hpp>

#include <d3d12.h>
#include <map>

namespace render {

struct ParamInfo {
	UINT RootParameterIndex;
	UINT Num32BitValuesToSet;
	UINT DestOffsetIn32BitValues;
};

struct ShaderProgram {
	ShaderProgram() = default;
	ShaderProgram(const std::string& name);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, const math::Vector3& value);
	void SetUniform(const std::string& name, const math::Vector4& value);
	void SetUniform(const std::string& name, const math::Matrix3& value);
	void SetUniform(const std::string& name, const math::Matrix4& value);
	void Use();

	winapi::ComPtr<ID3D12RootSignature> root_signature;
	winapi::ComPtr<ID3D12PipelineState> pipeline_state;

private:
	void InitParamsInfo(winapi::ComPtr<ID3DBlob> root_sig_blob,
		winapi::ComPtr<ID3DBlob> vertex_shader_blob, winapi::ComPtr<ID3DBlob> pixel_shader_blob);
	void FindContantParam(UINT RootIndex, const D3D12_ROOT_CONSTANTS& param, winapi::ComPtr<ID3DBlob> shader_blob);
	ParamInfo GetParamInfo(const std::string& name) const;
	std::map<std::string, ParamInfo> params_map;
};

}
