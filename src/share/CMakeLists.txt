file(GLOB SPINDULYS_HEADERS
	spindulys.h
	spindulys/*h
	spindulys/math/*.h
)

add_library(
	spindulysShare INTERFACE
)

target_include_directories(
	spindulysShare INTERFACE
	./
)

target_link_libraries(
	spindulysShare INTERFACE
	spdlog
)
