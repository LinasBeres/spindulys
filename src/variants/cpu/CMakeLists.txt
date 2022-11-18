file(GLOB CPU_HEADERS
	bsdf/*.h
	bsdf/smooth/*.h
	bsdf/rough/*.h
	camera/*.h
	geometry/*.h
	integrator/*.h
	lights/*.h
	render/*.h
	scene/*.h
	utils/*.h
	spindulysCPU.h
)

file(GLOB CPU_SOURCES
	bsdf/*.cpp
	bsdf/smooth/*.cpp
	bsdf/rough/*.cpp
	camera/*.cpp
	geometry/*.cpp
	integrator/*.cpp
	lights/*.cpp
	render/*.cpp
	scene/*.cpp
)

add_library(
	spindulysCPU
	${CPU_SOURCES}
	${CPU_HEADERS}
	${PROJECT_SHADERS}
	${PROJECT_SCENES}
	${PROJECT_CONFIGS}
)

target_include_directories(
	spindulysCPU PUBLIC
	./
)

target_link_libraries(
	spindulysCPU
	spindulysBase
	embree_common
	${EMBREE_LIBRARY}
	${TBB_LIBRARIES}
	spindulysShare
)
