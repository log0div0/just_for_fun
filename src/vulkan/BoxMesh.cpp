
#include "BoxMesh.hpp"
#include "Context.hpp"

namespace vulkan {

BoxMesh::BoxMesh() {
	vk::DeviceSize buffer_size = sizeof(box_vertices[0]) * box_vertices.size();

	Buffer staging_buffer(buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	uint8_t* p = (uint8_t*)staging_buffer.memory.mapMemory(0, buffer_size);
	memcpy(p, box_vertices.data(), buffer_size);
	staging_buffer.memory.unmapMemory();

	mesh_buffer = Buffer(buffer_size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	staging_buffer.copy_to(mesh_buffer);
}

void BoxMesh::Draw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	g_context->current_image->CommitAll();

	g_context->current_image->command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shader.GetPipeline());
	g_context->current_image->command_buffer.bindVertexBuffers(0, {*mesh_buffer}, {0});
	// g_context->current_image->command_buffer.draw((uint32_t)box_vertices.size(), 1, 0, 0);
}

}
