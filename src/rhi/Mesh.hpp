
#pragma once

#include "ShaderProgram.hpp"
#include "../Vertex.hpp"

namespace rhi
{

struct Mesh {
	static std::pair<std::vector<Vertex>, std::vector<uint32_t>> LoadFromFile(const fs::path& path);

	virtual ~Mesh() = 0 {}

	virtual void Draw(ShaderProgram& shader_program) = 0;
};

}
