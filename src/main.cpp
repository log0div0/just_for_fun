
#include <iostream>
#include <clipp.h>
#include "App.hpp"

int main(int argc, char** argv) {
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

	App app(assets_dir);
	app.Run();

	return 0;
}
