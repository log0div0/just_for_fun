cmake_minimum_required(VERSION 3.0)

file(GLOB SRCS "*.cpp" "*.hpp" "rhi/*.cpp" "rhi/*.hpp")
include_directories(common)

add_definitions(-DGLFW_INCLUDE_NONE)

if (WIN32)
	add_definitions(-DGLFW_EXPOSE_NATIVE_WIN32)
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
endif()

add_executable(just_for_fun ${SRCS})
target_link_libraries(just_for_fun stb tinyobjloader)

if (WIN32)
	target_link_libraries(just_for_fun winapi)
endif()

add_custom_target(asserts_dir_link
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE}/assets)
add_dependencies(just_for_fun asserts_dir_link)

if (NOT CMAKE_GENERATOR_PLATFORM MATCHES "^Gaming\\.Xbox")
	target_link_libraries(just_for_fun glfw imgui)
	if (WIN32)
		# for timeBeginPeriod/timeEndPeriod functions
		target_link_libraries(just_for_fun winmm)
	endif()
endif()

if (ENABLE_OPENGL)
	target_link_libraries(just_for_fun glad)
endif()

if (ENABLE_DX12)
	if (CMAKE_GENERATOR_PLATFORM MATCHES "^Gaming\\.Xbox")
		target_link_libraries(just_for_fun xgameplatform xgameruntime)
	endif()
	if (CMAKE_GENERATOR_PLATFORM STREQUAL "Gaming.Xbox.Scarlett.x64")
		target_link_libraries(just_for_fun d3d12_xs dxcompiler_xs)
		set(DXC_EXE $ENV{GXDKLatest}/bin/Scarlett/dxc.exe)
		set(SHADER_BINARY_EXT ".scarlett.bin")
	elseif (CMAKE_GENERATOR_PLATFORM STREQUAL "Gaming.Xbox.XBoxOne.x64")
		target_link_libraries(just_for_fun d3d12_x dxcompiler_x)
		set(DXC_EXE $ENV{GXDKLatest}/bin/XboxOne/dxc.exe)
		set(SHADER_BINARY_EXT ".xboxone.bin")
	elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
		target_link_libraries(just_for_fun d3d12 dxcompiler dxgi)
		set(DXC_EXE "$ENV{SystemDrive}/Program Files (x86)/Windows Kits/10/bin/${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}/x64/dxc.exe")
		set(SHADER_BINARY_EXT ".bin")
	else()
		message(FATAL_ERROR something is definitely wrong)
	endif()

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
		set(SHADER_DST "${SHADER_SRC}${SHADER_BINARY_EXT}")
		add_custom_command(
			OUTPUT ${SHADER_DST}
			COMMAND ${DXC_EXE} -T ${TARGET_PROFILE} -E main ${SHADER_SRC} -Fo ${SHADER_DST} -rootsig-define ROOT_SIG -Od -Zi -Zss -Qstrip_debug -Fd ${CMAKE_SOURCE_DIR}/assets/shaders/hlsl/pdb/
			DEPENDS ${SHADER_SRC}
		)
		add_custom_target(HLSL_${SHADER_FILE_NAME} DEPENDS ${SHADER_DST})
		add_dependencies(just_for_fun HLSL_${SHADER_FILE_NAME})
	endforeach()
endif()

if (ENABLE_VULKAN)
	target_link_libraries(just_for_fun vulkan-1 spirv_reflect)

	file(GLOB GLSL_SRCS
		"${CMAKE_SOURCE_DIR}/assets/shaders/glsl/*.vert"
		"${CMAKE_SOURCE_DIR}/assets/shaders/glsl/*.frag")

	foreach(SHADER_SRC ${GLSL_SRCS})
		get_filename_component(SHADER_FILE_NAME ${SHADER_SRC} NAME)
		set(SHADER_DST "${SHADER_SRC}.bin")
		add_custom_command(
			OUTPUT ${SHADER_DST}
			COMMAND $ENV{VK_SDK_PATH}/bin/glslc.exe ${SHADER_SRC} -o ${SHADER_DST} -O0 -g
			DEPENDS ${SHADER_SRC}
		)
		add_custom_target(GLSL_${SHADER_FILE_NAME} DEPENDS ${SHADER_DST})
		add_dependencies(just_for_fun GLSL_${SHADER_FILE_NAME})
	endforeach()
endif()