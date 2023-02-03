find_package(SDL2 REQUIRED)

include_directories(
	${SDL2_INCLUDE_DIRS}
)

set(SDL2_INCLUDE_DIRS "${SDL2_INCLUDE_DIRS}" CACHE INTERNAL "SDL2_INCLUDE_DIRS")
set(SDL2_LIBRARIES "${SDL2_LIBRARIES}" CACHE INTERNAL "SDL2_LIBRARIES")