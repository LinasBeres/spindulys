add_subdirectory(glfw)
add_subdirectory(nativefiledialog-extended)

# Setup internal dependencies
include(SetupCxxopts)
include(SetupEmbreeCommon)
include(SetupGLAD)
include(SetupImGui)
include(SetupMinitrace)
include(SetupSpdlog)
include(SetupTinyEXR)
include(SetupTinyOBJLoader)
