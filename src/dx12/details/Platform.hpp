
#pragma once

#include <winapi/ComPtr.hpp>
#include <winapi/Event.hpp>

#if defined(_GAMING_XBOX_SCARLETT)
#include <d3d12_xs.h>
#include <d3d12shader_xs.h>
#include <d3dx12_xs.h>
#include <dxcapi_xs.h>
#elif defined(_GAMING_XBOX_XBOXONE)
#include <d3d12_x.h>
#include <d3d12shader_x.h>
#include <d3dx12_x.h>
#include <dxcapi_x.h>
#else
#include <d3d12.h>
#include <d3d12shader.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <dxcapi.h>
#endif

#ifndef IID_GRAPHICS_PPV_ARGS
#define IID_GRAPHICS_PPV_ARGS(x) IID_PPV_ARGS(x)
#endif
