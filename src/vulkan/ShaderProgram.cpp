#include "ShaderProgram.hpp"
#include "Context.hpp"
#include "../Utils.hpp"

namespace vulkan {

static std::vector<uint8_t> LoadFile(const std::string& path) {
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file " + path);
	}

	size_t file_size = (size_t) file.tellg();
	std::vector<uint8_t> buffer(file_size);

	file.seekg(0);
	file.read((char*)buffer.data(), file_size);

	return buffer;
}

static vk::raii::ShaderModule LoadShaderModule(const std::string& path) {
	std::vector<uint8_t> code = LoadFile(path);
	vk::ShaderModuleCreateInfo create_info{
		.sType = vk::StructureType::eShaderModuleCreateInfo,
		.codeSize = code.size(),
		.pCode = reinterpret_cast<const uint32_t*>(code.data()),
	};
	return vk::raii::ShaderModule(g_context->device, create_info);
}

ShaderProgram::ShaderProgram(const std::string& name)
{
	vertex_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (name + ".vert.bin"));
	fragment_shader = LoadShaderModule(GetAssetsDir() / "shaders" / "glsl" / (name + ".frag.bin"));
}

}
