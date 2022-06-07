
#pragma once

#include <winapi/ComPtr.hpp>
#include <winapi/Event.hpp>

#if defined(_GAMING_XBOX_SCARLETT)
#include <d3d12_xs.h>
#elif defined(_GAMING_XBOX_XBOXONE)
#include <d3d12_x.h>
#else
#include <d3d12.h>
#include <dxgi1_6.h>
#endif
