#pragma once

#include <math/Vector.hpp>
#include <math/Matrix.hpp>

#include "../rhi/ShaderProgram.hpp"
#include "Texture2D.hpp"

namespace opengl {

struct ShaderProgram: rhi::ShaderProgram {
	ShaderProgram(const std::string& name);

	virtual void SetParam(const std::string& name, rhi::Texture2D& value_rhi) override;

	mogl::ShaderProgram shader_program;

private:
	int GetTextureUnit(const std::string& name);

	std::unordered_map<std::string, int> units_map;

	void PopulateUniformBindings();
};

}
