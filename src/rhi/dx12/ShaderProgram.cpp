#include "ShaderProgram.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"
#include "../Vertex.hpp"
#include "../../Utils.hpp"

#include <d3dcompiler.h>
#include <d3dx12.h>

using namespace winapi;

namespace rhi {

ComPtr<ID3DBlob> LoadShader(const std::string& name) {
	fs::path path = g_assets_dir / "shaders" / "hlsl" / (name + ".bin");
	ComPtr<ID3DBlob> blob;
	ThrowIfFailed(D3DReadFileToBlob(path.wstring().c_str(), &blob));
	return blob;
}

ShaderProgram::ShaderProgram(const std::string& name) {
	ComPtr<ID3DBlob> vertex_shader_blob = LoadShader(name + ".vert");
	ComPtr<ID3DBlob> pixel_shader_blob = LoadShader(name + ".frag");;

	D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",   0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(Vertex, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_ROOT_SIGNATURE_FLAGS root_signature_flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	std::array<CD3DX12_ROOT_PARAMETER1, 1> root_parameters;
	root_parameters.at(0).InitAsConstants(sizeof(math::Matrix4) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC root_signature_description;
	root_signature_description.Init_1_1(root_parameters.size(), root_parameters.data(), 0, nullptr, root_signature_flags);

	ComPtr<ID3DBlob> root_signature_blob;
	ComPtr<ID3DBlob> error_blob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&root_signature_description,
		D3D_ROOT_SIGNATURE_VERSION_1_1, &root_signature_blob, &error_blob));
	ThrowIfFailed(context->device->CreateRootSignature(0, root_signature_blob->GetBufferPointer(),
		root_signature_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature)));

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} pipeline_state_stream;

	pipeline_state_stream.pRootSignature = root_signature;
	pipeline_state_stream.InputLayout = { input_layout, _countof(input_layout) };
	pipeline_state_stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipeline_state_stream.VS = CD3DX12_SHADER_BYTECODE(vertex_shader_blob);
	pipeline_state_stream.PS = CD3DX12_SHADER_BYTECODE(pixel_shader_blob);
	pipeline_state_stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipeline_state_stream.RTVFormats = D3D12_RT_FORMAT_ARRAY {
		.RTFormats = { SwapChain::FORMAT },
		.NumRenderTargets = 1,
	};

	D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc = {
	    sizeof(pipeline_state_stream), &pipeline_state_stream
	};
	ThrowIfFailed(context->device->CreatePipelineState(&pipeline_state_stream_desc, IID_PPV_ARGS(&pipeline_state)));
}

void ShaderProgram::SetUniform(const std::string& name, float value) {

}

void ShaderProgram::SetUniform(const std::string& name, int value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector3& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector4& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix3& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix4& value) {
	if (name == "MVP") {
		context->command_list->SetGraphicsRoot32BitConstants(0, sizeof(value) / 4, &value, 0);
	}
}

void ShaderProgram::Use() {
	context->command_list->SetPipelineState(pipeline_state);
	context->command_list->SetGraphicsRootSignature(root_signature);
}

}
