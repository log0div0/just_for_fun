cmake_minimum_required(VERSION 3.0)

list(APPEND SRC_DIRS "*.cpp")
include_directories(common)
list(APPEND SRC_DIRS "common/**/*.cpp")

if (NOT DEFINED RHI)
	set(RHI "OPENGL")
endif()
string(TOUPPER ${RHI} RHI)
if (RHI STREQUAL "OPENGL")
	add_definitions(-DOPENGL)
	include_directories(opengl)
	list(APPEND SRC_DIRS "opengl/**/*.cpp")
elseif(RHI STREQUAL "VULKAN")
	add_definitions(-DVULKAN)
	include_directories(vulkan)
	list(APPEND SRC_DIRS "vulkan/**/*.cpp")
elseif(RHI STREQUAL "DX12")
	add_definitions(-DDX12)
	add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
	include_directories(dx12)
	list(APPEND SRC_DIRS "dx12/**/*.cpp")
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

if (WIN32)
	file(GLOB HLSL_SRCS
		"${CMAKE_SOURCE_DIR}/assets/shaders/hlsl/*.vert"
		"${CMAKE_SOURCE_DIR}/assets/shaders/hlsl/*.frag")

	foreach(SHADER_SRC ${HLSL_SRCS})
		get_filename_component(SHADER_FILE_NAME ${SHADER_SRC} NAME)
		get_filename_component(SHADER_FILE_EXT ${SHADER_SRC} EXT)
		if (SHADER_FILE_EXT STREQUAL ".vert")
			set(TARGET_PROFILE "vs_6_0")
		else()
			set(TARGET_PROFILE "ps_6_0")
		endif()
		set(SHADER_DST "${SHADER_SRC}.bin")
		add_custom_command(
			OUTPUT ${SHADER_DST}
			COMMAND ${CMAKE_SOURCE_DIR}/3rd_party/dxc/bin/x64/dxc.exe -T ${TARGET_PROFILE} -E main ${SHADER_SRC} -Fo ${SHADER_DST}
			DEPENDS ${SHADER_SRC}
		)
		add_custom_target(${SHADER_FILE_NAME} DEPENDS ${SHADER_DST})
		add_dependencies(just_for_fun ${SHADER_FILE_NAME})
	endforeach()
endif()
