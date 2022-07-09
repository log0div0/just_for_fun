
#include "BoxMesh.hpp"
#include "Context.hpp"

namespace vulkan {

BoxMesh::BoxMesh() {
	size_t buffer_size = sizeof(box_vertices[0]) * box_vertices.size();

	Buffer staging_buffer((uint8_t*)box_vertices.data(), buffer_size, vk::BufferUsageFlagBits::eTransferSrc);

	mesh_buffer = Buffer(buffer_size,
		vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer,
		vk::MemoryPropertyFlagBits::eDeviceLocal);

	staging_buffer.copy_to(mesh_buffer);
}

void BoxMesh::Draw(rhi::ShaderProgram& shader_rhi) {
	auto& shader = static_cast<ShaderProgram&>(shader_rhi);

	g_context->CommitAll();

	g_context->current_frame->command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, shader.GetPipeline());
	g_context->current_frame->command_buffer.bindVertexBuffers(0, {*mesh_buffer}, {0});
	g_context->current_frame->command_buffer.draw((uint32_t)box_vertices.size(), 1, 0, 0);
}

}
