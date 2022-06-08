
#include <iostream>
#include <clipp.h>
#include <scope_guard.hpp>
#include "App.hpp"

#ifdef WITH_OPENGL
#include "opengl/Context.hpp"
#define RHI_OPENGL "opengl"
#endif

#ifdef WITH_DX12
#include "dx12/Context.hpp"
#define RHI_DX12 "dx12"
#endif

#ifdef WITH_VULKAN
#include "vulkan/Context.hpp"
#define RHI_VULKAN "vulkan"
#endif

namespace rhi {
Context* g_context = nullptr;
}

int do_main(int argc, char** argv) {
	using namespace clipp;

	std::string rhi_name;

	std::vector<std::string> wrong;

	auto cli = (
		value("rhi_name", rhi_name) % "'dx12' or 'opengl' or 'vulkan'",
		any_other(wrong)
	);

	auto res = parse(argc, argv, cli);

	if (wrong.size()) {
		for (const std::string& arg: wrong) {
			std::cerr << "Error: '" << arg << "' is not a valid argument" << std::endl;
		}
		std::cout << "Usage:" << std::endl << usage_lines(cli, "testo") << std::endl;
		return -1;
	}

	if (!res) {
		std::cerr << "Error: invalid command line arguments" << std::endl;
		std::cout << "Usage:" << std::endl << usage_lines(cli, "testo") << std::endl;
		return -1;
	}

#ifndef _GAMING_XBOX
	auto GLFW = glfw::init();

#ifdef WITH_OPENGL
	if (rhi_name == RHI_OPENGL) {
		glfw::WindowHints{  .contextVersionMajor = 4,
							.contextVersionMinor = 6,
							.openglProfile = glfw::OpenGlProfile::Core }.apply();
	} else {
#endif
		glfw::WindowHints{  .clientApi = glfw::ClientApi::None }.apply();
#ifdef WITH_OPENGL
	}
#endif
#endif

	Window window(rhi_name);

#ifdef WITH_OPENGL
	if (rhi_name == RHI_OPENGL) {
		rhi::g_context = new opengl::Context(window);
	}
#endif
#ifdef WITH_DX12
	if (rhi_name == RHI_DX12) {
		rhi::g_context = new dx12::Context(window);
	}
#endif
#ifdef WITH_VULKAN
	if (rhi_name == RHI_VULKAN) {
		rhi::g_context = new vulkan::Context(window);
	}
#endif
	if (rhi::g_context == nullptr) {
		throw std::runtime_error("Invalid rhi");
	}

	SCOPE_EXIT {
		delete rhi::g_context;
	};

#if defined(WIN32) && !defined(_GAMING_XBOX)
	timeBeginPeriod(1);
	SCOPE_EXIT {
		timeEndPeriod(1);
	};
#endif

	{
		App app(window);
		app.Run();
	}

	return 0;
}

int main(int argc, char** argv) {
	try {
		return do_main(argc, argv);
	}
	catch (const std::exception& error) {
		std::cerr << "UNHANDLED EXCEPTION!" << std::endl;
		std::cerr << error.what() << std::endl;
		return 1;
	}
}