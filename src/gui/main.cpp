// Spindulys by Linas Beresna

#define STB_IMAGE_IMPLEMENTATION

#include <filesystem>

#include <cxxopts.hpp>

#include "window.h"

int main(int argc, char** argv)
{
	// Tracing Starting
	// ----------------------------------
	mtr_init("trace.json");
	MTR_META_PROCESS_NAME("spindulys");
	MTR_META_THREAD_NAME("main thread");
	MTR_BEGIN("GUI", "main()");
	// ----------------------------------

	spdlog::trace("main()");
	cxxopts::Options options("spindulys", "A C++ GLFW path tracer");

	options.add_options()
		("s,scene", "Path to Scene", cxxopts::value<std::string>()->default_value(""))
		("l,level", "Logging level from trace to off (0-6)", cxxopts::value<int>()->default_value("2"))
		("h,help", "Print usage")
	;

	const cxxopts::ParseResult result = options.parse(argc, argv);

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(SPINDULYS_EXIT_GOOD);
	}

	if (result.count("level"))
	{
		const int level = result["level"].as<int>();
		if (level > 0 && level < 7)
			spdlog::set_level(static_cast<spdlog::level::level_enum>(level));
		else
			spdlog::warn("Level {} is not within range 0 to 6, therefore resorting to default of 2 (info).", level);
	}

	std::string scenePath;
	if (result.count("scene"))
	{
		scenePath = result["scene"].as<std::string>();
		const std::filesystem::path path(scenePath);

		if (!std::filesystem::exists(path))
		{
			spdlog::error("Filepath {} does not exist.\n Please input one that does. Exiting.", path.string());
			exit(SPINDULYS_EXIT_BAD_PATH);
		}
		if (path.is_relative())
			scenePath = std::filesystem::current_path() / scenePath;
	}

	spindulys::spindulysFrontend::spindulysBackendCPU::spindulysGUI::Window mainWindow;
	mainWindow.RenderWindow(scenePath);

	// Tracing Ending.
	// ----------------------------------
	MTR_END("GUI", "main()");
	MTR_INSTANT("main", "the end");
	mtr_flush();
	mtr_shutdown();
	// ----------------------------------

	spdlog::info("All good exiting now.");
	return SPINDULYS_EXIT_GOOD;
}
