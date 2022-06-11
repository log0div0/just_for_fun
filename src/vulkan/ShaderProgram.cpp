#include "ShaderProgram.hpp"
#include "Context.hpp"
#include "../Utils.hpp"
#include <spirv_reflect.h>

namespace vulkan {

ShaderProgram::ShaderProgram(const std::string& name)
{
	vertex_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (name + ".vert.bin"));
	fragment_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (name + ".frag.bin"));
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

}
