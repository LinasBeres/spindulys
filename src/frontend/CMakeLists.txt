file(GLOB FRONTEND_HEADERS
	spindulysFrontend.h
	camera/*.h
	geometry/*.h
	render/*.h
	scene/*.h
	scene/usdTranslators/*.h
	utils/*.h
)

file(GLOB FRONTEND_SOURCES
	camera/*.cpp
	geometry/*.cpp
	render/*.cpp
	scene/*.cpp
	scene/usdTranslators/*.cpp
)

add_library(
	spindulysFrontend
	${FRONTEND_SOURCES}
	${FRONTEND_HEADERS}
	${PROJECT_SHADERS}
	${PROJECT_SCENES}
	${PROJECT_CONFIGS}
)

target_include_directories(
	spindulysFrontend PUBLIC
	./
)

target_link_libraries(
	spindulysFrontend
	${TBB_LIBRARIES}
	${USD_LIBRARIES}
	spindulysShare
)
