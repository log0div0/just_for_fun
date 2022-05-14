#include "ShaderProgram.hpp"
#include "Context.hpp"

#include "details/Exceptions.hpp"
#include "../../Vertex.hpp"
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

	pipeline_state_stream.pRootSignature = g_context->root_signature;
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

	PopulateBindingsMap(vertex_shader_blob);
	PopulateBindingsMap(pixel_shader_blob);
}

void ShaderProgram::PopulateBindingsMap(ComPtr<ID3DBlob> shader_blob)
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

	// Constant buffers
	for (UINT i = 0; i < shader_desc.BoundResources; i++)
	{
		D3D12_SHADER_INPUT_BIND_DESC bind_desc;
		shader_reflection->GetResourceBindingDesc(i,&bind_desc);
		if (bind_desc.Type == D3D_SIT_CBUFFER || bind_desc.Type == D3D_SIT_TBUFFER)
		{
			ID3D12ShaderReflectionConstantBuffer* constant_buffer = shader_reflection->GetConstantBufferByName(bind_desc.Name);
			D3D12_SHADER_BUFFER_DESC cb_desc;
			constant_buffer->GetDesc(&cb_desc);

			for (UINT i = 0; i < cb_desc.Variables; i++)
			{
				ID3D12ShaderReflectionVariable* variable = constant_buffer->GetVariableByIndex(i);
				D3D12_SHADER_VARIABLE_DESC variable_desc;
				variable->GetDesc(&variable_desc);

				bindings_map.emplace(variable_desc.Name, ConstantBufferBindingInfo{bind_desc.BindPoint, variable_desc.StartOffset, variable_desc.Size});
			}
		}
		else if (bind_desc.Type == D3D_SIT_TEXTURE)
		{
			bindings_map.emplace(bind_desc.Name, TextureBindingInfo{bind_desc.BindPoint});
		}
	}
}

void ShaderProgram::SetParam(const std::string& name, const math::Vector3& value) {
	SetConstantParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Vector4& value) {
	SetConstantParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Matrix3& value) {
	SetConstantParam(name, (uint8_t*)&value, 3, sizeof(math::Vector3), sizeof(math::Vector4));
}

void ShaderProgram::SetParam(const std::string& name, const math::Matrix4& value) {
	SetConstantParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, Texture2D& value) {
	const BindingInfo& binding = GetBindingInfo(name);
	auto& texture_binding = std::get<TextureBindingInfo>(binding);
	g_context->CreateSRV(texture_binding.root_parameter_index, value);
}

const BindingInfo& ShaderProgram::GetBindingInfo(const std::string& name) const {
	auto it = bindings_map.find(name);
	if (it == bindings_map.end()) {
		throw std::runtime_error("Shader doesn't have param " + name);
	}
	return it->second;
}

void ShaderProgram::SetConstantParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride) {
	const BindingInfo& binding = GetBindingInfo(name);
	auto& cb_binding = std::get<ConstantBufferBindingInfo>(binding);
	ConstantBuffer& constant_buffer = g_context->constant_buffers.at(cb_binding.root_parameter_index);
	constant_buffer.Write(cb_binding.offset, src, num_rows, src_stride, dst_stride);
}

}
