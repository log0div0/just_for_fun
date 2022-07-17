#include "Mesh.hpp"
#include "../Utils.hpp"
#include <tiny_obj_loader.h>

namespace rhi
{

std::pair<std::vector<Vertex>, std::vector<uint32_t>> Mesh::LoadFromFile(const fs::path& path)
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	fs::path full_path = GetAssetsDir() / "meshes" / path;

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, full_path.c_str())) {
		throw std::runtime_error(err);
	}

	if (warn.size()) {
		std::cerr << warn << std::endl;
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.uv = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertices.push_back(vertex);
			indices.push_back(indices.size());
		}
	}

	return {vertices, indices};
}

}
