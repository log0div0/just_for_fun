#include "ShaderProgram.hpp"
#include "Exceptions.hpp"
#include "Context.hpp"
#include "render/Vertex.hpp"
#include "../../Utils.hpp"

#include <d3dcompiler.h>
#include <d3dx12.h>
#include <dxcapi.h>

using namespace winapi;

namespace render {

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

	ComPtr<ID3DBlob> root_sig_blob;
	ThrowIfFailed(D3DGetBlobPart(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), D3D_BLOB_ROOT_SIGNATURE, 0, &root_sig_blob));

	ThrowIfFailed(g_context->device->CreateRootSignature(0, root_sig_blob->GetBufferPointer(),
		root_sig_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature)));

	struct PipelineStateStream
	{
		CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
		CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
		CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
		CD3DX12_PIPELINE_STATE_STREAM_VS VS;
		CD3DX12_PIPELINE_STATE_STREAM_PS PS;
		CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencil;
		CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
		CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
	} pipeline_state_stream;

	CD3DX12_DEPTH_STENCIL_DESC depth_stencil = CD3DX12_DEPTH_STENCIL_DESC(CD3DX12_DEFAULT{});
	depth_stencil.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;

	CD3DX12_RASTERIZER_DESC rasterizer = CD3DX12_RASTERIZER_DESC(CD3DX12_DEFAULT{});
	rasterizer.CullMode = D3D12_CULL_MODE_FRONT;

	pipeline_state_stream.pRootSignature = root_signature;
	pipeline_state_stream.InputLayout = { input_layout, _countof(input_layout) };
	pipeline_state_stream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipeline_state_stream.VS = CD3DX12_SHADER_BYTECODE(vertex_shader_blob);
	pipeline_state_stream.PS = CD3DX12_SHADER_BYTECODE(pixel_shader_blob);
	pipeline_state_stream.Rasterizer = rasterizer;
	pipeline_state_stream.DepthStencil = depth_stencil;
	pipeline_state_stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipeline_state_stream.RTVFormats = D3D12_RT_FORMAT_ARRAY {
		.RTFormats = { SwapChain::FORMAT },
		.NumRenderTargets = 1,
	};

	D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc = {
	    sizeof(pipeline_state_stream), &pipeline_state_stream
	};
	ThrowIfFailed(g_context->device->CreatePipelineState(&pipeline_state_stream_desc, IID_PPV_ARGS(&pipeline_state)));

	InitParamsInfo(root_sig_blob,
		vertex_shader_blob, pixel_shader_blob);
}

void ShaderProgram::InitParamsInfo(ComPtr<ID3DBlob> root_sig_blob, ComPtr<ID3DBlob> vertex_shader_blob, ComPtr<ID3DBlob> pixel_shader_blob)
{
	ComPtr<ID3D12VersionedRootSignatureDeserializer> root_sig_deserializer;
	ThrowIfFailed(D3D12CreateVersionedRootSignatureDeserializer(root_sig_blob->GetBufferPointer(),
		root_sig_blob->GetBufferSize(), IID_PPV_ARGS(&root_sig_deserializer)));

	const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* v_root_sig_desc = nullptr;
	ThrowIfFailed(root_sig_deserializer->GetRootSignatureDescAtVersion(D3D_ROOT_SIGNATURE_VERSION_1_1, &v_root_sig_desc));

	const D3D12_ROOT_SIGNATURE_DESC1* root_sig_desc = &v_root_sig_desc->Desc_1_1;

	for (uint32_t i = 0; i < root_sig_desc->NumParameters; ++i) {
		const D3D12_ROOT_PARAMETER1& param = root_sig_desc->pParameters[i];
		switch (param.ParameterType) {
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
				FindContantParam(i, param.Constants, vertex_shader_blob);
				FindContantParam(i, param.Constants, pixel_shader_blob);
				break;
			default:
				throw std::runtime_error("IMPLEMENT ME!!!!!!!!!!!!");
				break;
		}
	}
}

void ShaderProgram::FindContantParam(UINT RootIndex, const D3D12_ROOT_CONSTANTS& param, ComPtr<ID3DBlob> shader_blob)
{
	ComPtr<IDxcContainerReflection> container_reflection;
	ThrowIfFailed(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&container_reflection)));
	ThrowIfFailed(container_reflection->Load((IDxcBlob*)(ID3DBlob*)shader_blob));
	UINT32 shader_index = 0;
	ThrowIfFailed(container_reflection->FindFirstPartKind(DXC_PART_DXIL, &shader_index));
	ComPtr<ID3D12ShaderReflection> shader_reflection;
	ThrowIfFailed(container_reflection->GetPartReflection(shader_index, IID_PPV_ARGS(&shader_reflection)));
	D3D12_SHADER_DESC shader_desc = {};
	ThrowIfFailed(shader_reflection->GetDesc(&shader_desc));
	for (int i = 0; i < shader_desc.BoundResources; ++i)
	{
		D3D12_SHADER_INPUT_BIND_DESC bind_desc;
		shader_reflection->GetResourceBindingDesc(i, &bind_desc);
		if ((bind_desc.BindPoint == param.ShaderRegister) &&
			(bind_desc.Space == param.RegisterSpace))
		{
			for (int i = 0; i < shader_desc.ConstantBuffers; ++i)
			{
				ID3D12ShaderReflectionConstantBuffer* constant_buffer = shader_reflection->GetConstantBufferByIndex(i);
				D3D12_SHADER_BUFFER_DESC buffer_desc = {};
				constant_buffer->GetDesc(&buffer_desc);
				if (std::string(buffer_desc.Name) == std::string(bind_desc.Name))
				{
					for (int i = 0; i < buffer_desc.Variables; ++i)
					{
						ID3D12ShaderReflectionVariable* var = constant_buffer->GetVariableByIndex(i);
						D3D12_SHADER_VARIABLE_DESC var_desc = {};
						var->GetDesc(&var_desc);
						ParamInfo info {
							.RootParameterIndex = RootIndex,
							.Num32BitValuesToSet = var_desc.Size / 4,
							.DestOffsetIn32BitValues = var_desc.StartOffset / 4,
						};
						params_map[var_desc.Name] = info;
					}
					break;
				}
			}
			break;
		}
	}
}

void ShaderProgram::SetUniform(const std::string& name, float value) {
}

void ShaderProgram::SetUniform(const std::string& name, int value) {
}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector3& value) {
	ParamInfo param_info = GetParamInfo(name);
	g_context->command_list->SetGraphicsRoot32BitConstants(param_info.RootParameterIndex, 3, &value, param_info.DestOffsetIn32BitValues);
}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector4& value) {
	ParamInfo param_info = GetParamInfo(name);
	g_context->command_list->SetGraphicsRoot32BitConstants(param_info.RootParameterIndex, 4, &value, param_info.DestOffsetIn32BitValues);
}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix3& value) {
	ParamInfo param_info = GetParamInfo(name);
	for (int i = 0; i < 3; ++i) {
		g_context->command_list->SetGraphicsRoot32BitConstants(param_info.RootParameterIndex, 3, (float*)&value + (i*3), param_info.DestOffsetIn32BitValues + (i*4));
	}
}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix4& value) {
	ParamInfo param_info = GetParamInfo(name);
	g_context->command_list->SetGraphicsRoot32BitConstants(param_info.RootParameterIndex, 16, &value, param_info.DestOffsetIn32BitValues);
}

void ShaderProgram::Use() {
	g_context->command_list->SetPipelineState(pipeline_state);
	g_context->command_list->SetGraphicsRootSignature(root_signature);
}

ParamInfo ShaderProgram::GetParamInfo(const std::string& name) const {
	auto it = params_map.find(name);
	if (it == params_map.end()) {
		throw std::runtime_error("Shader doesn't have param " + name);
	}
	return it->second;
}

}
