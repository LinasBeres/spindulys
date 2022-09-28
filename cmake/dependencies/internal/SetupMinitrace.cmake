file(GLOB THIRDPARTY_MINITRACE_SOURCES
    "minitrace/minitrace.c"
)

add_library(
	minitrace
	${THIRDPARTY_MINITRACE_SOURCES}
)

target_include_directories(
	minitrace PUBLIC
	"minitrace"
)
