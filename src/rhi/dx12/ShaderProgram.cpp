#include "ShaderProgram.hpp"
#include "Exceptions.hpp"
#include "../../Utils.hpp"

#include <d3dcompiler.h>

using namespace winapi;

namespace rhi {

ComPtr<ID3DBlob> LoadShader(const std::string& name) {
	fs::path path = g_assets_dir / "shaders" / "hlsl" / (name + ".bin");
	ComPtr<ID3DBlob> blob;
	ThrowIfFailed(D3DReadFileToBlob(path.wstring().c_str(), &blob));
	return blob;
}

ShaderProgram::ShaderProgram(const std::string& name) {
	ComPtr<ID3DBlob> vertex_shader_blob = LoadShader(name + ".vert");
	ComPtr<ID3DBlob> pixel_shader_blob = LoadShader(name + ".frag");;
}

void ShaderProgram::SetUniform(const std::string& name, float value) {

}

void ShaderProgram::SetUniform(const std::string& name, int value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector3& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Vector4& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix3& value) {

}

void ShaderProgram::SetUniform(const std::string& name, const math::Matrix4& value) {

}

void ShaderProgram::Use() {

}

}
