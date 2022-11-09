# Shared between everything
add_subdirectory(share)

# Base which the variants will implement
add_subdirectory(base)

# Implementations of the renderer
add_subdirectory(variants)

# Visualisers
add_subdirectory(gui)
