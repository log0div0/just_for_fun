#pragma once

#ifdef OPENGL
#include "rhi/opengl/OpenGL.hpp"
#elif VULKAN
#include "rhi/vulkan/Vulkan.hpp"
#elif DX12
#include "rhi/dx12/DX12.hpp"
#endif
