#pragma once

#ifdef OPENGL
#include "rhi/opengl/OpenGL.hpp"
#elif VULKAN
#include "rhi/vulkan/Vulkan.hpp"
#elif DX12
#include "rhi/dx12/Context.hpp"
#include "rhi/dx12/BoxMesh.hpp"
#include "rhi/dx12/ShaderProgram.hpp"
#include "rhi/dx12/Texture2D.hpp"
#endif
