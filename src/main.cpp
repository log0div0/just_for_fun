
#include <iostream>
#include <clipp.h>
#include "App.hpp"

fs::path g_assets_dir;

int do_main(int argc, char** argv) {
	using namespace clipp;

	std::string assets_dir;

	std::vector<std::string> wrong;

	auto cli = (
		value("assets_dir", assets_dir) % "Path to a directory with assets (shaders, models, etc)",
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

	auto GLFW = glfw::init();

	glfw::WindowHints{  .contextVersionMajor = 4,
						.contextVersionMinor = 6,
						.openglProfile = glfw::OpenGlProfile::Core }.apply();

	glfw::Window window {640, 480, "Hello World"};
	glfw::makeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to initialize GLAD");
	}

	g_assets_dir = assets_dir;

	App app(std::move(window));
	app.Run();

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