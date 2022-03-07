cmake_minimum_required(VERSION 3.0)

list(APPEND SRC_DIRS "*.cpp")
list(APPEND SRC_DIRS "rhi/*.cpp")

if (NOT DEFINED RHI)
	set(RHI "OPENGL")
endif()
string(TOUPPER ${RHI} RHI)
if (RHI STREQUAL "OPENGL")
	add_definitions(-DOPENGL)
	list(APPEND SRC_DIRS "rhi/opengl/*.cpp")
elseif(RHI STREQUAL "VULKAN")
	add_definitions(-DVULKAN)
	list(APPEND SRC_DIRS "rhi/vulkan/*.cpp")
elseif(RHI STREQUAL "DX12")
	add_definitions(-DDX12)
	add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
	list(APPEND SRC_DIRS "rhi/dx12/*.cpp")
endif()

add_definitions(-DGLFW_INCLUDE_NONE)

file(GLOB SRCS ${SRC_DIRS})

add_executable(just_for_fun ${SRCS})
target_link_libraries(just_for_fun glfw stb imgui)

if (RHI STREQUAL "OPENGL")
	target_link_libraries(just_for_fun glad)
elseif(RHI STREQUAL "VULKAN")
elseif(RHI STREQUAL "DX12")
	target_link_libraries(just_for_fun D3D12 DXGI)
endif()

if (WIN32)
	target_link_libraries(just_for_fun Winmm)
endif()
