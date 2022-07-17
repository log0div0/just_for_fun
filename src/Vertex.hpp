
#pragma once

#include <math/Vector.hpp>
#include <vector>

struct Vertex {
	math::Vector3 pos;
	math::Vector2 uv;
	math::Vector3 normal;
};

inline bool operator==(const Vertex& a, const Vertex& b) {
	return a.pos == b.pos
		&& a.uv == b.uv
		&& a.normal == b.normal;
}

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(const Vertex& vertex) const {
			// TODO: improve and test me
			return hash<math::Vector3>()(vertex.pos) ^
				   hash<math::Vector2>()(vertex.uv) ^
				   hash<math::Vector3>()(vertex.normal);
		}
	};
}
