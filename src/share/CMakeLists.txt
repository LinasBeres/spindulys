file(GLOB SPINDULYS_HEADERS
	spindulys.h
	spindulys/*h
	spindulys/math/*.h
)

add_library(
	spindulysShare INTERFACE
)

target_sources(spindulysShare INTERFACE
	FILE_SET HEADERS
	BASE_DIRS ./
	FILES ${SPINDULYS_HEADERS}
)
