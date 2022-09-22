file(GLOB FRONTEND_HEADERS
	spindulysFrontend.h
	camera/*.h
	geometry/*.h
	render/*.h
	scene/*.h
	scene/objTranslators/*.h
	utils/*.h
)

file(GLOB FRONTEND_SOURCES
	camera/*.cpp
	geometry/*.cpp
	render/*.cpp
	scene/*.cpp
	scene/objTranslators/*.cpp
)

if(USD_FOUND)
	file(GLOB USD_FRONTEND_HEADERS scene/usdTranslators/*.h)
	list(APPEND FRONTEND_HEADERS ${USD_FRONTEND_HEADERS})

	file(GLOB USD_FRONTEND_SOURCES scene/usdTranslators/*cpp)
	list(APPEND FRONTEND_SOURCES ${USD_FRONTEND_SOURCES})
endif()

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
	tinyObjLoader
)
