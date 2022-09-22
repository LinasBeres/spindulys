// Spindulys by Linas Beresna

#define STB_IMAGE_IMPLEMENTATION

#include <filesystem>

#include <cxxopts.hpp>

#include "window.h"

int main(int argc, char** argv)
{
	cxxopts::Options options("spindulys", "A C++ GLFW path tracer");

	options.add_options()
		("s,scene", "Path to Scene", cxxopts::value<std::string>()->default_value(""))
		("h,help", "Print usage")
	;

	const cxxopts::ParseResult result = options.parse(argc, argv);

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(SPINDULYS_EXIT_GOOD);
	}

	std::string scenePath;
	if (result.count("scene"))
	{
		scenePath = result["scene"].as<std::string>();
		const std::filesystem::path path(scenePath);

		if (!std::filesystem::exists(path))
		{
			std::cerr << "Filepath " << path << " does not exist.\n"
				<< "Please input one that does.\n";
			exit(SPINDULYS_EXIT_BAD_PATH);
		}
		if (path.is_relative())
			scenePath = std::filesystem::current_path() / scenePath;
	}

	spindulys::spindulysFrontend::spindulysBackendCPU::spindulysGUI::Window mainWindow;
	mainWindow.RenderWindow(scenePath);

	return SPINDULYS_EXIT_GOOD;
}
