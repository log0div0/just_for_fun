#include "ShaderProgram.hpp"
#include "Context.hpp"

#include "details/Exceptions.hpp"
#include "../Vertex.hpp"
#include "../Utils.hpp"

#include <winapi/Functions.hpp>

using namespace winapi;

namespace dx12 {

#define COMPILE_SHADERS_IN_RUNTIME 0

#if COMPILE_SHADERS_IN_RUNTIME

class DXCIncludeHandler : public IDxcIncludeHandler
{
public:
	DXCIncludeHandler(const fs::path& include_path_) :
		include_path(include_path_)
	{
		ThrowIfFailed(DxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&dxc_library)));
	}

	virtual HRESULT STDMETHODCALLTYPE LoadSource(_In_ LPCWSTR pFilename, _COM_Outptr_result_maybenull_ IDxcBlob** ppIncludeSource) override
	{
		fs::path file_path = include_path / std::wstring(pFilename);
		IDxcBlobEncoding* blob = nullptr;
		UINT32 code_page = CP_UTF8;
		HRESULT hr = dxc_library->CreateBlobFromFile(file_path.wstring().c_str(), &code_page, &blob);
		if (SUCCEEDED(hr)) {
			*ppIncludeSource = blob;
		}
		return hr;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR* __RPC_FAR* ppvObject) override {
		throw std::runtime_error("What do you want??");
	}

	ULONG STDMETHODCALLTYPE AddRef(void) override { return 0; }
	ULONG STDMETHODCALLTYPE Release(void) override { return 0; }

private:
	fs::path include_path;
	ComPtr<IDxcLibrary> dxc_library;
};

#endif

ComPtr<IDxcBlob> LoadShader(const std::string& name)
{
#if COMPILE_SHADERS_IN_RUNTIME
	fs::path path = GetAssetsDir() / "shaders" / "hlsl" / name;
	std::wstring target_profile;
	if (path.extension() == ".vert")
	{
		target_profile = TEXT("vs_6_0");
	}
	else if (path.extension() == ".frag")
	{
		target_profile = TEXT("ps_6_0");
	}
	else
	{
		throw std::runtime_error("Invalid file extension");
	}
	std::string src = LoadTextFile(path);

	ComPtr<IDxcCompiler3> compiler;
	ThrowIfFailed(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)));

	DXCIncludeHandler include_handler(path.parent_path());

	ComPtr<IDxcResult> result;

	DxcBuffer src_buffer = {
		src.data(),
		src.size(),
		CP_UTF8
	};

	std::vector<LPCWSTR> args;

	args.push_back(TEXT("-T"));
	args.push_back(target_profile.c_str());
	args.push_back(TEXT("-E"));
	args.push_back(TEXT("main"));
	args.push_back(TEXT("-Od"));
	args.push_back(TEXT("-Zi"));
	args.push_back(TEXT("-Zss"));
	args.push_back(TEXT("-Qstrip_debug"));

	ThrowIfFailed(compiler->Compile(
		&src_buffer,
		args.data(),
		args.size(),
		&include_handler,
		IID_PPV_ARGS(&result)
	));

	ComPtr<IDxcBlob> Code;
	result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&Code), nullptr);

	ComPtr<IDxcBlobUtf8> Error;
	result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&Error), nullptr);

	if (Code->GetBufferSize() == 0) {
		throw std::runtime_error(Error->GetStringPointer());
	} else {
		if (Error->GetStringLength() > 0) {
			std::cerr << Error->GetStringPointer() << std::endl;
		}
	}

	ComPtr<IDxcBlob> pdb;
	ComPtr<IDxcBlobUtf16> pdb_name;
	result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdb_name);

	fs::path pdb_path = path.parent_path() / "pdb" / std::wstring(pdb_name->GetStringPointer());

	SaveBinaryFile(pdb_path, (uint8_t*)pdb->GetBufferPointer(), pdb->GetBufferSize());

	return Code;

#else
	std::string ext;
#ifdef _GAMING_XBOX_SCARLETT
	ext = ".scarlett.bin";
#elif _GAMING_XBOX_XBOXONE
	ext = ".xboxone.bin";
#else
	ext = ".bin";
#endif
	fs::path path = GetAssetsDir() / "shaders" / "hlsl" / (name + ext);
	std::vector<uint8_t> data = LoadBinaryFile(path);
	ComPtr<IDxcUtils> utils;
	ThrowIfFailed(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils)));
	IDxcBlobEncoding* blob = nullptr;
	ThrowIfFailed(utils->CreateBlob(data.data(), data.size(), 0, &blob));
	return ComPtr<IDxcBlob>(blob);
#endif
}

ShaderProgram::ShaderProgram(const std::string& name) {
	ComPtr<IDxcBlob> vertex_shader_blob = LoadShader(name + ".vert");
	ComPtr<IDxcBlob> pixel_shader_blob = LoadShader(name + ".frag");

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
	pipeline_state_stream.VS = CD3DX12_SHADER_BYTECODE((ID3DBlob*)vertex_shader_blob.Get());
	pipeline_state_stream.PS = CD3DX12_SHADER_BYTECODE((ID3DBlob*)pixel_shader_blob.Get());
	pipeline_state_stream.Rasterizer = rasterizer;
	pipeline_state_stream.DepthStencil = depth_stencil;
	pipeline_state_stream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	pipeline_state_stream.RTVFormats = D3D12_RT_FORMAT_ARRAY {
		.RTFormats = { SwapChain::BACK_BUFFER_FORMAT },
		.NumRenderTargets = 1,
	};

	D3D12_PIPELINE_STATE_STREAM_DESC pipeline_state_stream_desc = {
		sizeof(pipeline_state_stream), &pipeline_state_stream
	};
	ThrowIfFailed(g_context->device->CreatePipelineState(&pipeline_state_stream_desc, IID_GRAPHICS_PPV_ARGS(&pipeline_state)));

	PopulateBindingsMap(vertex_shader_blob);
	PopulateBindingsMap(pixel_shader_blob);
}

void ShaderProgram::PopulateBindingsMap(ComPtr<IDxcBlob> shader_blob)
{
	ComPtr<IDxcContainerReflection> container_reflection;
	ThrowIfFailed(DxcCreateInstance(CLSID_DxcContainerReflection, IID_PPV_ARGS(&container_reflection)));
	ThrowIfFailed(container_reflection->Load(shader_blob.Get()));
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

				uniform_bindings.emplace(variable_desc.Name, rhi::UniformBinding{bind_desc.BindPoint, variable_desc.StartOffset});
			}
		}
		else if (bind_desc.Type == D3D_SIT_TEXTURE)
		{
			texture_bindings.emplace(bind_desc.Name, bind_desc.BindPoint);
		}
	}
}

void ShaderProgram::SetParam(const std::string& name, rhi::Texture2D& value_rhi) {
	auto& value = static_cast<Texture2D&>(value_rhi);
	int binding = texture_bindings.at(name);
	g_context->CreateSRV(binding, value);
}

}
