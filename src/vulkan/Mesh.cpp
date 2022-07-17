
#include "Mesh.hpp"
#include "Context.hpp"

namespace vulkan {

Mesh::Mesh(const fs::path& path) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::tie(vertices, indices) = LoadFromFile(path);

	{
		size_t buffer_size = sizeof(vertices[0]) * vertices.size();

		Buffer staging_buffer((uint8_t*)vertices.data(), buffer_size, vk::BufferUsageFlagBits::eTransferSrc);

		vertex_buffer = Buffer(buffer_size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal);

		staging_buffer.copy_to(vertex_buffer);
	}

	{
		size_t buffer_size = sizeof(indices[0]) * indices.size();

		Buffer staging_buffer((uint8_t*)indices.data(), buffer_size, vk::BufferUsageFlagBits::eTransferSrc);

		index_buffer = Buffer(buffer_size,
			vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal);

		staging_buffer.copy_to(index_buffer);
	}
}

void Mesh::Draw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	uint32_t index_count = index_buffer.info.size / sizeof(uint32_t);

	g_context->CommitAll();

	g_context->current_frame->command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shader.GetPipeline());
	g_context->current_frame->command_buffer.bindVertexBuffers(0, {*vertex_buffer}, {0});
	g_context->current_frame->command_buffer.bindIndexBuffer(*index_buffer, 0, vk::IndexType::eUint32);
	g_context->current_frame->command_buffer.drawIndexed(index_count, 1, 0, 0, 0);
}

}
