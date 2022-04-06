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

	{
		ComPtr<ID3D12VersionedRootSignatureDeserializer> root_sig_deserializer;
		ThrowIfFailed(D3D12CreateVersionedRootSignatureDeserializer(root_sig_blob->GetBufferPointer(),
			root_sig_blob->GetBufferSize(), IID_PPV_ARGS(&root_sig_deserializer)));

		const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* v_root_sig_desc = nullptr;
		ThrowIfFailed(root_sig_deserializer->GetRootSignatureDescAtVersion(D3D_ROOT_SIGNATURE_VERSION_1_1, &v_root_sig_desc));

		const D3D12_ROOT_SIGNATURE_DESC1* root_sig_desc = &v_root_sig_desc->Desc_1_1;

		InitParamsInfo(root_sig_desc, vertex_shader_blob);
		InitParamsInfo(root_sig_desc, pixel_shader_blob);
	}
}

void ShaderProgram::InitParamsInfo(const D3D12_ROOT_SIGNATURE_DESC1* root_sig_desc, ComPtr<ID3DBlob> shader_blob)
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

	auto find_resource_binding = [&](const D3D_SHADER_INPUT_TYPE shader_input_type, const UINT shader_register, const UINT register_space) -> std::optional<D3D12_SHADER_INPUT_BIND_DESC> {
		for (int i = 0; i < shader_desc.BoundResources; ++i)
		{
			D3D12_SHADER_INPUT_BIND_DESC bind_desc;
			shader_reflection->GetResourceBindingDesc(i, &bind_desc);
			if ((bind_desc.BindPoint == shader_register) &&
				(bind_desc.Space == register_space) &&
				(bind_desc.Type == shader_input_type))
			{
				return bind_desc;
			}
		}
		return {}; // resource is in another shader
	};

	auto find_constant_buffer = [&](const D3D_SHADER_INPUT_TYPE shader_input_type, const UINT shader_register, const UINT register_space) -> std::pair<ID3D12ShaderReflectionConstantBuffer*, D3D12_SHADER_BUFFER_DESC> {
		std::optional<D3D12_SHADER_INPUT_BIND_DESC> bind_desc = find_resource_binding(shader_input_type, shader_register, register_space);
		if (bind_desc) {
			for (int i = 0; i < shader_desc.ConstantBuffers; ++i)
			{
				ID3D12ShaderReflectionConstantBuffer* constant_buffer = shader_reflection->GetConstantBufferByIndex(i);
				D3D12_SHADER_BUFFER_DESC buffer_desc = {};
				constant_buffer->GetDesc(&buffer_desc);
				if (std::string(bind_desc->Name) == std::string(buffer_desc.Name))
				{
					return {constant_buffer, buffer_desc};
				}
			}
		}
		return {nullptr, {}};
	};

	for (uint32_t i = 0; i < root_sig_desc->NumParameters; ++i) {
		const D3D12_ROOT_PARAMETER1& param = root_sig_desc->pParameters[i];
		ParamInfo info = {};
		info.type = param.ParameterType;
		info.root_parameter_index = i;
		switch (param.ParameterType) {
			case D3D12_ROOT_PARAMETER_TYPE_CBV:
				{
					auto [constant_buffer, buffer_desc] = find_constant_buffer(D3D_SIT_CBUFFER, param.Descriptor.ShaderRegister, param.Descriptor.RegisterSpace);

					if (constant_buffer) {
						for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; ++i)
						{
							// These will remain in upload heap because we use them only once per frame.
							CD3DX12_HEAP_PROPERTIES heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
							CD3DX12_RESOURCE_DESC res_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer_desc.Size);
							g_context->device->CreateCommittedResource(
								&heap_props,
								D3D12_HEAP_FLAG_NONE,
								&res_desc,
								D3D12_RESOURCE_STATE_GENERIC_READ,
								nullptr,
								IID_PPV_ARGS(&info.constant_buffers[i]));
						}
					}
					[[fallthrough]];
				}
			case D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS:
				{
					auto [constant_buffer, buffer_desc] = find_constant_buffer(D3D_SIT_CBUFFER, param.Constants.ShaderRegister, param.Constants.RegisterSpace);

					if (constant_buffer) {
						for (int i = 0; i < buffer_desc.Variables; ++i)
						{
							ID3D12ShaderReflectionVariable* var = constant_buffer->GetVariableByIndex(i);
							D3D12_SHADER_VARIABLE_DESC var_desc = {};
							var->GetDesc(&var_desc);
							info.offset = var_desc.StartOffset;
							params_map[var_desc.Name] = info;
						}
					}
				}
				break;
			case D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE:
				for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; ++i)
				{
					info.descriptor_tables[i] = g_context->srv_desc_heap.alloc_range(NUM_FRAMES_IN_FLIGHT);
				}
				for (int i = 0; i < param.DescriptorTable.NumDescriptorRanges; ++i) {
					info.offset = i;
					const D3D12_DESCRIPTOR_RANGE1& range = param.DescriptorTable.pDescriptorRanges[i];
					if (range.RangeType == D3D12_DESCRIPTOR_RANGE_TYPE_SRV) {
						std::optional<D3D12_SHADER_INPUT_BIND_DESC> bind_desc = find_resource_binding(D3D_SIT_TEXTURE, range.BaseShaderRegister, range.RegisterSpace);
						if (bind_desc) {
							params_map[bind_desc->Name] = info;
						}
					} else {
						throw std::runtime_error("TODOOO");
					}
				}
				break;
			default:
				throw std::runtime_error("IMPLEMENT ME!!!!!!!!!!!!");
				break;
		}
	}
}

void ShaderProgram::Use() {
	g_context->command_list->SetPipelineState(pipeline_state);
	g_context->command_list->SetGraphicsRootSignature(root_signature);
}

void ShaderProgram::SetParam(const std::string& name, const math::Vector3& value) {
	SetParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Vector4& value) {
	SetParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const math::Matrix3& value) {
	SetParam(name, (uint8_t*)&value, 3, sizeof(math::Vector3), sizeof(math::Vector4));
}

void ShaderProgram::SetParam(const std::string& name, const math::Matrix4& value) {
	SetParam(name, (uint8_t*)&value, 1, sizeof(value), sizeof(value));
}

void ShaderProgram::SetParam(const std::string& name, const uint8_t* src, int num_rows, int src_stride, int dst_stride) {
	ParamInfo param_info = GetParamInfo(name);
	if (param_info.type == D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS) {
		for (int i = 0; i < num_rows; ++i) {
			g_context->command_list->SetGraphicsRoot32BitConstants(param_info.root_parameter_index, src_stride / 4, src + (i*src_stride), (param_info.offset + i*dst_stride)/4);
		}
	} else if (param_info.type == D3D12_ROOT_PARAMETER_TYPE_CBV) {
		ID3D12Resource* constant_buffer = param_info.constant_buffers.at(g_context->current_frame->index);

		void* p = nullptr;
		constant_buffer->Map(0, nullptr, &p);
		uint8_t* dst = static_cast<uint8_t*>(p) + param_info.offset;
		for (int i = 0; i < num_rows; ++i) {
			memcpy(dst + i*dst_stride, src + i*src_stride, src_stride);
		}
		constant_buffer->Unmap(0, nullptr);

	} else {
		throw std::runtime_error("Something went wrong");
	}
}

void ShaderProgram::SetParam(const std::string& name, Texture2D& value) {
	ParamInfo param_info = GetParamInfo(name);
	if (param_info.type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
		D3D12_CPU_DESCRIPTOR_HANDLE dst = param_info.descriptor_tables.at(g_context->current_frame->index).cpu;
		dst.ptr += param_info.offset * g_context->device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		g_context->device->CopyDescriptorsSimple(1, dst, value.texture_srv_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	} else {
		throw std::runtime_error("Something went wrong");
	}
}

void ShaderProgram::Done() {
	for (auto& kv: params_map) {
		ParamInfo& param_info = kv.second;
		if (param_info.type == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			g_context->command_list->SetGraphicsRootDescriptorTable(param_info.root_parameter_index, param_info.descriptor_tables.at(g_context->current_frame->index).gpu);
		} else if (param_info.type == D3D12_ROOT_PARAMETER_TYPE_CBV) {
			ID3D12Resource* constant_buffer = param_info.constant_buffers.at(g_context->current_frame->index);
			g_context->command_list->SetGraphicsRootConstantBufferView(param_info.root_parameter_index, constant_buffer->GetGPUVirtualAddress());
		}
	}
}

ParamInfo ShaderProgram::GetParamInfo(const std::string& name) const {
	auto it = params_map.find(name);
	if (it == params_map.end()) {
		throw std::runtime_error("Shader doesn't have param " + name);
	}
	return it->second;
}

}
