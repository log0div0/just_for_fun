#include "Context.hpp"
#include "details/Exceptions.hpp"

#include <d3dx12.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_dx12.h"

using namespace winapi;

namespace dx12 {

void Context::ImGuiInit() {
	auto srv_handle = view_heap.alloc();
	ImGui_ImplGlfw_InitForOther(window.GetGLFW(), true);
	ImGui_ImplDX12_Init(
		device,
		NUM_FRAMES_IN_FLIGHT,
		SwapChain::FORMAT,
		view_heap.heap,
		srv_handle.cpu,
		srv_handle.gpu
	);
}

void Context::ImGuiShutdown() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplGlfw_Shutdown();
}

void Context::ImGuiNewFrame() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplGlfw_NewFrame();
}

void Context::ImGuiRender() {
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list);
}


void Context::InitDevice()
{
	ComPtr<ID3D12Debug> debug;
	ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)));
	debug->EnableDebugLayer();

	ThrowIfFailed(D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));

	ComPtr<ID3D12InfoQueue> info_queue = device.QueryInterface<ID3D12InfoQueue>();
	ThrowIfFailed(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
	ThrowIfFailed(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));
	ThrowIfFailed(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true));
}

void Context::InitRootSignature()
{
	D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	CD3DX12_DESCRIPTOR_RANGE1 srv_range(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_TABLE_SIZE, 0);
	CD3DX12_DESCRIPTOR_RANGE1 cbv_range(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, CBV_TABLE_SIZE, 0);
	CD3DX12_DESCRIPTOR_RANGE1 sampler_range(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, SAMPLER_TABLE_SIZE, 0);

	CD3DX12_ROOT_PARAMETER1 parameters[NUM_OF_ROOT_SIG_TABLES];

	parameters[SRV_TABLE_INDEX].InitAsDescriptorTable(1, &srv_range);
	parameters[CBV_TABLE_INDEX].InitAsDescriptorTable(1, &cbv_range);
	parameters[SAMPLER_TABLE_INDEX].InitAsDescriptorTable(1, &sampler_range);

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC desc;
	desc.Init_1_1(NUM_OF_ROOT_SIG_TABLES, parameters, 0, nullptr, flags);

	ComPtr<ID3DBlob> root_sig_blob;
	ComPtr<ID3DBlob> error_blob;
	ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_1, &root_sig_blob, &error_blob));
	ThrowIfFailed(device->CreateRootSignature(0, root_sig_blob->GetBufferPointer(),
	    root_sig_blob->GetBufferSize(), IID_PPV_ARGS(&root_signature)));
}

void Context::InitHeaps()
{
	view_heap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, VIEW_HEAP_SIZE);
	sampler_heap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, SAMPLER_HEAP_SIZE);
	rtv_heap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, RTV_HEAP_SIZE);
	dsv_heap = DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DSV_HEAP_SIZE);
}

void Context::InitQueues()
{
	direct_queue = CommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);
	copy_queue = CommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
}

void Context::InitSwapchain(int w, int h)
{
	swap_chain = SwapChain(w, h, NUM_FRAMES_IN_FLIGHT);
	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
	scissor_rect = CD3DX12_RECT(0, 0, w, h);
}

void Context::InitDepthStencilBuffer(int w, int h) {
	dsv_handle = dsv_heap.alloc().cpu;

	{
		CD3DX12_HEAP_PROPERTIES heap_props(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, w, h,
			1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		D3D12_CLEAR_VALUE clear_value = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.DepthStencil = { 0.0f, 0 },
		};

		ThrowIfFailed(device->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clear_value,
			IID_PPV_ARGS(&depth_stencil_buffer)
		));
	}

	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc = {
			.Format = DXGI_FORMAT_D32_FLOAT,
			.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
			.Flags = D3D12_DSV_FLAG_NONE,
			.Texture2D = {
				.MipSlice = 0,
			},
		};


		device->CreateDepthStencilView(depth_stencil_buffer, &desc, dsv_handle);
	}
}

void Context::InitFrames()
{
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i] = Frame(i);
	}
}

Context* g_context = nullptr;

Context::Context(Window& window_): window(window_) {
	g_context = this;
	auto [w, h] = window.GetWindowSize();
	InitDevice();
	InitRootSignature();
	InitHeaps();
	InitQueues();
	InitSwapchain(w, h);
	InitDepthStencilBuffer(w, h);
	InitFrames();
	window.OnWindowResize([&](int w, int h) {
		Resize(w, h);
	});
}

Context::~Context() {
	g_context = nullptr;
}

void Context::Resize(int w, int h) {
	if (!w || !h) {
		return;
	}
	WaitIdle();
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i].render_target_buffer = {};
	}
	depth_stencil_buffer = {};
	swap_chain.ResizeBuffers(w, h);
	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h));
	scissor_rect = CD3DX12_RECT(0, 0, w, h);
	for (int i = 0; i < NUM_FRAMES_IN_FLIGHT; i++) {
		frames[i].InitRenderTargetBuffer(i);
	}
	InitDepthStencilBuffer(w, h);
}

void Context::WaitIdle() {
	direct_queue.WaitIdle();
	copy_queue.WaitIdle();
}

void Context::Clear() {
	int frame_idx = swap_chain.AcquireNextBufferIndex();
	current_frame = &frames[frame_idx];
	direct_queue.WaitForFenceValue(current_frame->fence_value);

	current_frame->constant_buffers_refs.clear();
	current_frame->descriptor_table_refs.clear();

	current_frame->command_allocator->Reset();
	if (command_list.IsNull()) {
		ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, current_frame->command_allocator, NULL, IID_PPV_ARGS(&command_list)));
	} else {
		command_list->Reset(current_frame->command_allocator, NULL);
	}

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		current_frame->render_target_buffer,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	command_list->ResourceBarrier(1, &barrier);

	const float clear_color[4] = { 0.2f, 0.3f, 0.3f, 1.0f };
	float depth = 0.0f;

	std::array<ID3D12DescriptorHeap*, 2> heaps = {view_heap.heap, sampler_heap.heap};

	command_list->ClearRenderTargetView(current_frame->rtv_handle, clear_color, 0, NULL);
	command_list->ClearDepthStencilView(dsv_handle, D3D12_CLEAR_FLAG_DEPTH, depth, 0, 0, nullptr);
	command_list->OMSetRenderTargets(1, &current_frame->rtv_handle, FALSE, &dsv_handle);
	command_list->SetDescriptorHeaps(heaps.size(), heaps.data());
	command_list->RSSetViewports(1, &viewport);
	command_list->RSSetScissorRects(1, &scissor_rect);
	command_list->SetGraphicsRootSignature(root_signature);
}

void Context::Present() {
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		current_frame->render_target_buffer,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	command_list->ResourceBarrier(1, &barrier);
	command_list->Close();

	direct_queue.Execute(command_list);
	swap_chain.Present();
	current_frame->fence_value = direct_queue.Signal();
}

void Context::CommitCBs() {
	DescriptorTable cbv_table(&view_heap, CBV_TABLE_SIZE);
	for (size_t i = 0; i < uniform_buffers.size(); ++i) {
		rhi::UniformBuffer& ub = uniform_buffers[i];
		ConstantBuffer cb(ub);
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbv {
			.BufferLocation = cb.GetGPUVirtualAddress(),
			.SizeInBytes = cb.GetSize(),
		};
		device->CreateConstantBufferView(&cbv, cbv_table.GetCPUHandle(i));
		current_frame->constant_buffers_refs.push_back(std::move(cb));
		ub.Reset();
	}
	command_list->SetGraphicsRootDescriptorTable(CBV_TABLE_INDEX, cbv_table.GetBaseGPUHandle());
	current_frame->descriptor_table_refs.push_back(std::move(cbv_table));
}

void Context::CommitResources() {
	CommitCBs();
	CommitSRVs();
	CommitSamplers();
}

void Context::CreateSRV(size_t root_parameter_index, Texture2D& texture) {
	if (srv_table.GetSize() == 0) {
		srv_table = DescriptorTable(&view_heap, SRV_TABLE_SIZE);
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
		.Format = Texture2D::img_format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D = {
			.MostDetailedMip = 0,
			.MipLevels = 1,
		}
	};
	device->CreateShaderResourceView(texture.resource, &srv_desc, srv_table.GetCPUHandle(root_parameter_index));
	srv_table_map[root_parameter_index] = true;
}

void Context::CommitSRVs() {
	if (srv_table.GetSize() == 0) {
		return;
	}
	for (size_t i = 0; i < SRV_TABLE_SIZE; ++i) {
		if (srv_table_map[i] == false) {
			D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {
				.Format = Texture2D::img_format,
				.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
				.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
				.Texture2D = {
					.MostDetailedMip = 0,
					.MipLevels = 1,
				}
			};
			device->CreateShaderResourceView(nullptr, &srv_desc, srv_table.GetCPUHandle(i));
		}
	}
	command_list->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, srv_table.GetBaseGPUHandle());
	current_frame->descriptor_table_refs.push_back(std::move(srv_table));
	srv_table_map = {};
}

void Context::CommitSamplers() {
	if (sampler_table.GetSize() != 0) {
		command_list->SetGraphicsRootDescriptorTable(SAMPLER_TABLE_INDEX, sampler_table.GetBaseGPUHandle());
		return;
	}
	sampler_table = DescriptorTable(&sampler_heap, SAMPLER_TABLE_SIZE);
	for (size_t i = 0; i < SAMPLER_TABLE_SIZE; ++i) {
		D3D12_SAMPLER_DESC desc = {
			.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT,
			.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			.MipLODBias = 0,
			.MaxAnisotropy = 16,
			.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER,
			.BorderColor = {0.0f, 0.0f, 0.0f, 0.0f},
			.MinLOD = 0,
			.MaxLOD = D3D12_FLOAT32_MAX,
		};
		device->CreateSampler(&desc, sampler_table.GetCPUHandle(i));
	}
	command_list->SetGraphicsRootDescriptorTable(SAMPLER_TABLE_INDEX, sampler_table.GetBaseGPUHandle());
}

}
