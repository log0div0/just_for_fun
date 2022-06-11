#include "ShaderProgram.hpp"
#include "Context.hpp"
#include "../Utils.hpp"
// #include <spir_reflect.h>

namespace vulkan {

ShaderProgram::ShaderProgram(const std::string& name)
{
	vertex_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (name + ".vert.bin"));
	fragment_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (name + ".frag.bin"));
}

vk::raii::ShaderModule ShaderProgram::LoadShaderModule(const std::string& path) {
	std::vector<uint8_t> code = LoadBinaryFile(path);


	// spir_reflect::ShaderModule reflect(code);


	vk::ShaderModuleCreateInfo create_info{
		.sType = vk::StructureType::eShaderModuleCreateInfo,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data()),
	};
	return vk::raii::ShaderModule(g_context->device, create_info);
}

}
