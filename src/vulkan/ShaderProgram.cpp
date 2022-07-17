#include "ShaderProgram.hpp"
#include "Context.hpp"
#include "../Utils.hpp"
#include <spirv_reflect.h>

namespace vulkan {

std::vector<vk::VertexInputBindingDescription> GetBindingDescriptions() {
	vk::VertexInputBindingDescription binding_description{
		.binding = 0,
		.stride = sizeof(Vertex),
		.inputRate = vk::VertexInputRate::eVertex,
	};
	return {binding_description};
}

std::vector<vk::VertexInputAttributeDescription> GetAttributeDescriptions() {
	vk::VertexInputAttributeDescription pos {
		.location = 0,
		.binding = 0,
		.format = vk::Format::eR32G32B32Sfloat,
		.offset = offsetof(Vertex, pos),
	};
	vk::VertexInputAttributeDescription uv {
		.location = 1,
		.binding = 0,
		.format = vk::Format::eR32G32Sfloat,
		.offset = offsetof(Vertex, uv),
	};
	vk::VertexInputAttributeDescription normal {
		.location = 2,
		.binding = 0,
		.format = vk::Format::eR32G32B32Sfloat,
		.offset = offsetof(Vertex, normal),
	};
	return {pos, uv, normal};
}

ShaderProgram::ShaderProgram(const fs::path& path)
{
	vertex_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (path.string() + ".vert.bin"));
	fragment_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (path.string() + ".frag.bin"));
}

void VerifyReflect(SpvReflectResult result) {
	if (result != SPV_REFLECT_RESULT_SUCCESS) {
		throw std::runtime_error("spirv reflect error");
	}
}

vk::raii::ShaderModule ShaderProgram::LoadShaderModule(const std::string& path) {
	std::vector<uint8_t> code = LoadBinaryFile(path);


	spv_reflect::ShaderModule reflect(code);
	{
		uint32_t num = 0;
		VerifyReflect(reflect.EnumerateDescriptorBindings(&num, nullptr));
		std::vector<SpvReflectDescriptorBinding*> descriptors(num, nullptr);
		VerifyReflect(reflect.EnumerateDescriptorBindings(&num, descriptors.data()));
		for (auto descriptor: descriptors) {
			if (descriptor->resource_type == SPV_REFLECT_RESOURCE_FLAG_CBV) {
				auto& block = descriptor->block;
				for (size_t i = 0; i < block.member_count; ++i) {
					auto& member = block.members[i];
					uniform_bindings.emplace(member.name, rhi::UniformBinding{
						descriptor->binding,
						member.offset
					});
				}
			} else if (descriptor->resource_type & SPV_REFLECT_RESOURCE_FLAG_SRV) {
				texture_bindings.emplace(descriptor->name, descriptor->binding);
			}
		}
	}

	vk::ShaderModuleCreateInfo create_info{
		.sType = vk::StructureType::eShaderModuleCreateInfo,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data()),
	};
	return vk::raii::ShaderModule(g_context->device, create_info);
}

void ShaderProgram::SetParam(const std::string& name, rhi::Texture2D& value_rhi) {
	auto& value = static_cast<Texture2D&>(value_rhi);
	int binding = texture_bindings.at(name);
	g_context->SetSRV(binding, value);
}

vk::Pipeline ShaderProgram::GetPipeline() {
	if (w == g_context->swapchain_info.imageExtent.width &&
		h == g_context->swapchain_info.imageExtent.height)
	{
		return *pipeline;
	}

	w = g_context->swapchain_info.imageExtent.width;
	h = g_context->swapchain_info.imageExtent.height;

	vk::PipelineShaderStageCreateInfo vertex_stage_info {
		.sType = vk::StructureType::ePipelineShaderStageCreateInfo,
		.stage = vk::ShaderStageFlagBits::eVertex,
		.module = *vertex_shader,
		.pName = "main",
	};

	vk::PipelineShaderStageCreateInfo fragment_stage_info {
		.sType = vk::StructureType::ePipelineShaderStageCreateInfo,
		.stage = vk::ShaderStageFlagBits::eFragment,
		.module = *fragment_shader,
		.pName = "main",
	};

	std::vector<vk::VertexInputBindingDescription> binding_descriptions = GetBindingDescriptions();
	std::vector<vk::VertexInputAttributeDescription> attribute_descriptions = GetAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertex_input_info {
		.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo,
		.vertexBindingDescriptionCount = (uint32_t)binding_descriptions.size(),
		.pVertexBindingDescriptions = binding_descriptions.data(),
		.vertexAttributeDescriptionCount = (uint32_t)attribute_descriptions.size(),
		.pVertexAttributeDescriptions = attribute_descriptions.data(),
	};

	vk::PipelineInputAssemblyStateCreateInfo input_assembly_info {
		.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo,
		.topology = vk::PrimitiveTopology::eTriangleList,
	};

	vk::Viewport viewport{
		.x = 0.0f,
		.y = (float)g_context->swapchain_info.imageExtent.height,
		.width = (float)g_context->swapchain_info.imageExtent.width,
		.height = -(float)g_context->swapchain_info.imageExtent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	vk::Rect2D scissor{
		.offset = {0, 0},
		.extent = g_context->swapchain_info.imageExtent,
	};

	std::vector<vk::Viewport> viewports = {viewport};
	std::vector<vk::Rect2D> scissors = {scissor};

	vk::PipelineViewportStateCreateInfo viewport_info {
		.sType = vk::StructureType::ePipelineViewportStateCreateInfo,
		.viewportCount = (uint32_t)viewports.size(),
		.pViewports = viewports.data(),
		.scissorCount = (uint32_t)scissors.size(),
		.pScissors = scissors.data(),
	};

	vk::PipelineRasterizationStateCreateInfo rasterization_info {
		.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo,
		.polygonMode = vk::PolygonMode::eFill,
		.cullMode = vk::CullModeFlagBits::eBack,
		.frontFace = vk::FrontFace::eCounterClockwise,
		.lineWidth = 1.0f,
	};

	vk::PipelineMultisampleStateCreateInfo multisample_info {
		.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo,
		.rasterizationSamples = vk::SampleCountFlagBits::e1,
	};

	vk::PipelineColorBlendAttachmentState color_blend_attachment{
		.blendEnable = VK_FALSE,
		.colorWriteMask =
			vk::ColorComponentFlagBits::eR |
			vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB |
			vk::ColorComponentFlagBits::eA,
	};

	std::vector<vk::PipelineColorBlendAttachmentState> color_blend_attachments = {color_blend_attachment};

	vk::PipelineColorBlendStateCreateInfo color_blend_info {
		.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo,
		.attachmentCount = (uint32_t)color_blend_attachments.size(),
		.pAttachments = color_blend_attachments.data(),
	};

	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages = {vertex_stage_info, fragment_stage_info};

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_state = {
		.depthTestEnable = true,
		.depthWriteEnable = true,
		.depthCompareOp = vk::CompareOp::eGreater,
	};

	vk::GraphicsPipelineCreateInfo pipeline_info {
		.sType = vk::StructureType::eGraphicsPipelineCreateInfo,

		.stageCount = (uint32_t)shader_stages.size(),
		.pStages = shader_stages.data(),

		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_assembly_info,
		.pViewportState = &viewport_info,
		.pRasterizationState = &rasterization_info,
		.pMultisampleState = &multisample_info,
		.pDepthStencilState = &depth_stencil_state,
		.pColorBlendState = &color_blend_info,
		.pDynamicState = nullptr,

		.layout = *g_context->pipeline_layout,

		.renderPass = *g_context->render_pass,
		.subpass = 0,
	};

	pipeline = vk::raii::Pipeline(g_context->device, nullptr, pipeline_info);

	return *pipeline;
}

}
