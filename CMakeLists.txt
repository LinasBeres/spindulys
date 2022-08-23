cmake_minimum_required(VERSION 3.12)

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
include(SetupEmbree)
include(SetupTBB)
include(SetupUSD)

# Include 3rd Party Internal
add_subdirectory(thirdparty)

# Setup Project Resources
file(GLOB PROJECT_SHADERS
    res/shaders/*.frag
    res/shaders/*.glsl
    res/shaders/*.vert
)

file(GLOB PROJECT_SCENES
    res/scenes/*.usd
    res/scenes/*.usda
    res/scenes/*.usdc
    res/scenes/*.usdz
)

file(GLOB PROJECT_CONFIGS
    CMakeLists.txt
    README.md
    .gitattributes
    .gitignore
    .gitmodules
)

# Include Source
add_subdirectory("src")
