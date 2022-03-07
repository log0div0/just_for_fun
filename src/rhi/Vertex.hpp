
#pragma once

#include <math/Vector.hpp>
#include <vector>

struct Vertex {
	math::Vector3 pos;
	math::Vector2 uv;
	math::Vector3 normal;
};

extern std::vector<Vertex> box_vertices;
extern std::vector<uint32_t> box_indices;