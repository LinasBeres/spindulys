cmake_minimum_required(VERSION 3.13)

project(spindulys
	VERSION 1.0
	DESCRIPTION "Renderer using C++, Embree and USD to achieve Path Tracing techniques on the CPU."
	HOMEPAGE_URL "https://github.com/LinasBeres/spindulys"
	LANGUAGES C CXX
	)

# Adding CMake modules
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	${CMAKE_SOURCE_DIR}/cmake/dependencies/external
	${CMAKE_SOURCE_DIR}/cmake/dependencies/internal
	${CMAKE_SOURCE_DIR}/cmake/modules
	${CMAKE_SOURCE_DIR}/cmake/setup
	)

# Project CMake setup
include(Options)
include(CXX)

# Include 3rd Party External
# include(SetupSDL2)
include(SetupEmbree)
include(SetupTBB)
include(SetupUSD)

# Include 3rd Party Internal
add_subdirectory(thirdparty)

if (USD_FOUND)
	add_compile_definitions(
		USING_USD=${USD_FOUND}
	)
endif()

# Setup Project Resources
file(GLOB PROJECT_SHADERS
	resources/shaders/*.frag
	resources/shaders/*.glsl
	resources/shaders/*.vert
	)

if(USD_FOUND)
	file(GLOB PROJECT_SCENES
		resources/scenes/*.usd
		resources/scenes/*.usda
		resources/scenes/*.usdc
		resources/scenes/*.usdz
		)
endif()

file(GLOB PROJECT_CONFIGS
	CMakeLists.txt
	README.md
	.gitattributes
	.gitignore
	)

# Include Source
add_subdirectory("src")
