file(GLOB BACKEND_CPU_HEADERS
	bsdf/*.h
	camera/*.h
	geometry/*.h
	integrator/direct.h
	integrator/integrator.h
	integrator/udpt.h
	lights/*.h
	render/*.h
	scene/*.h
	utils/*.h
	spindulysCPUBackend.h
)

file(GLOB BACKEND_CPU_SOURCES
	bsdf/*.cpp
	camera/*.cpp
	geometry/*.cpp
	integrator/direct.cpp
	integrator/integrator.cpp
	integrator/udpt.cpp
	lights/*.cpp
	render/*.cpp
	scene/*.cpp
)

add_library(
	spindulysCPUBackend
	${BACKEND_CPU_SOURCES}
	${BACKEND_CPU_HEADERS}
	${PROJECT_SHADERS}
	${PROJECT_SCENES}
	${PROJECT_CONFIGS}
)

target_include_directories(
	spindulysCPUBackend PUBLIC
	./
)

target_link_libraries(
	spindulysCPUBackend
	spindulysBase
	embree_common
	${EMBREE_LIBRARY}
	${TBB_LIBRARIES}
	spindulysShare
)
