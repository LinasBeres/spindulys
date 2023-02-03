file(GLOB GUI_HEADERS
	spindulysGUI.h
	window.h
	output_helper.h
	# opengl/*.h
)

file(GLOB GUI_SOURCES
	main.cpp
	window.cpp
	# opengl/*.cpp
)

add_executable(
	${PROJECT_NAME}
	${GUI_SOURCES}
	${GUI_HEADERS}
	${GUI_SHADERS}
	${GUI_SCENES}
	${GUI_CONFIGS}
)

add_definitions(-DGLFW_INCLUDE_NONE)

target_link_libraries(
	${PROJECT_NAME}
	glfw
	${SDL2_LIBRARIES}
	glad
	imgui
	tinyexr
	spindulysShare
	spindulysCPU
	cxxopts
	nfd
)

set(VERT_SHADER ${CMAKE_SOURCE_DIR}/resources/shaders/screenQuad.vert)
set(FRAG_SHADER ${CMAKE_SOURCE_DIR}/resources/shaders/screenQuad.frag)

add_compile_definitions(
	VERT_SHADER="${VERT_SHADER}"
	FRAG_SHADER="${FRAG_SHADER}"
	TEST_SCENE="${TEST_SCENE}"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
)
