file(GLOB BASE_HEADERS
	spindulysBase.h
	bsdf/*.h
	camera/*.h
	geometry/*.h
	lights/*.h
	render/*.h
	scene/*.h
	scene/objTranslators/*.h
	utils/*.h
)

file(GLOB BASE_SOURCES
	bsdf/*.cpp
	camera/*.cpp
	geometry/*.cpp
	render/*.cpp
	scene/*.cpp
	scene/objTranslators/*.cpp
)

if(USD_FOUND)
	file(GLOB USD_BASE_HEADERS scene/usdTranslators/*.h)
	list(APPEND BASE_HEADERS ${USD_BASE_HEADERS})

	file(GLOB USD_BASE_SOURCES scene/usdTranslators/*cpp)
	list(APPEND BASE_SOURCES ${USD_BASE_SOURCES})
endif()

add_library(
	spindulysBase
	${BASE_SOURCES}
	${BASE_HEADERS}
	${PROJECT_SHADERS}
	${PROJECT_SCENES}
	${PROJECT_CONFIGS}
)

target_include_directories(
	spindulysBase PUBLIC
	./
)

target_link_libraries(
	spindulysBase
	${TBB_LIBRARIES}
	${USD_LIBRARIES}
	spindulysShare
	tinyObjLoader
)
