file(GLOB THIRDPARTY_IMGUI_SOURCES
    imgui/*.cpp
)

add_compile_definitions(IMGUI_IMPL_OPENGL_LOADER_GLAD=1)

add_library(
    imgui
    ${THIRDPARTY_IMGUI_SOURCES}
)

target_include_directories(
    imgui PUBLIC
    "imgui"
)
target_link_libraries(
    imgui
    glad
    ${GLFW_LIBRARIES}
)
