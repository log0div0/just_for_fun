cmake_minimum_required(VERSION 3.0)

file(GLOB SRCS "*.cpp" "*.hpp")
include_directories(common)

add_definitions(-DGLFW_INCLUDE_NONE)

if (WIN32)
	add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
endif()

set(ENABLE_OPENGL 1)
set(ENABLE_VULKAN 1)
if (WIN32)
	set(ENABLE_DX12 1)
endif()

if (ENABLE_OPENGL)
	add_definitions(-DWITH_OPENGL)
	file(GLOB_RECURSE OPENGL_SRCS "opengl/*.cpp" "opengl/*.hpp")
	list(APPEND SRCS ${OPENGL_SRCS})
endif()

if (ENABLE_VULKAN)
	add_definitions(-DWITH_VULKAN)
	file(GLOB_RECURSE VULKAN_SRCS "vulkan/*.cpp" "vulkan/*.hpp")
	list(APPEND SRCS ${VULKAN_SRCS})

	add_definitions(
		-DVULKAN_HPP_NO_STRUCT_CONSTRUCTORS
	)
endif()

if (ENABLE_DX12)
	add_definitions(-DWITH_DX12)
	file(GLOB_RECURSE DX12_SRCS "dx12/*.cpp" "dx12/*.hpp")
	list(APPEND SRCS ${DX12_SRCS})

	# this is for a shader blob reflection
	configure_file(
		$ENV{VK_SDK_PATH}/bin/dxcompiler.dll
		${CMAKE_BINARY_DIR}/out/sbin/${CMAKE_BUILD_TYPE}/dxcompiler.dll
		COPYONLY)
endif()

add_executable(just_for_fun ${SRCS})
target_link_libraries(just_for_fun glfw stb imgui)

if (ENABLE_OPENGL)
	target_link_libraries(just_for_fun glad)
endif()

if (ENABLE_DX12)
	target_link_libraries(just_for_fun d3d12 dxgi dxcompiler)
endif()

if (WIN32)
	# for timeBeginPeriod/timeEndPeriod functions
	target_link_libraries(just_for_fun winmm)
endif()

if (ENABLE_DX12)
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
			COMMAND $ENV{VK_SDK_PATH}/bin/dxc.exe -T ${TARGET_PROFILE} -E main ${SHADER_SRC} -Fo ${SHADER_DST}
			DEPENDS ${SHADER_SRC}
		)
		add_custom_target(HLSL_${SHADER_FILE_NAME} DEPENDS ${SHADER_DST})
		add_dependencies(just_for_fun HLSL_${SHADER_FILE_NAME})
	endforeach()
endif()

if (0)
	file(GLOB GLSL_SRCS
		"${CMAKE_SOURCE_DIR}/assets/shaders/glsl/*.vert"
		"${CMAKE_SOURCE_DIR}/assets/shaders/glsl/*.frag")

	foreach(SHADER_SRC ${GLSL_SRCS})
		get_filename_component(SHADER_FILE_NAME ${SHADER_SRC} NAME)
		set(SHADER_DST "${SHADER_SRC}.bin")
		add_custom_command(
			OUTPUT ${SHADER_DST}
			COMMAND $ENV{VK_SDK_PATH}/bin/glslc.exe ${SHADER_SRC} -o ${SHADER_DST}
			DEPENDS ${SHADER_SRC}
		)
		add_custom_target(GLSL_${SHADER_FILE_NAME} DEPENDS ${SHADER_DST})
		add_dependencies(just_for_fun GLSL_${SHADER_FILE_NAME})
	endforeach()
endif()