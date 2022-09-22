set(USD_ROOT $ENV{REZ_USD_ROOT})

find_package(USD)

include_directories(
    ${USD_INCLUDE_DIRS}
)

set (USD_LIBRARIES
    ar
    arch
    gf
    js
    kind
    ndr
    pcp
    plug
    sdf
    sdr
    tf
    trace
    usd
    usdGeom
    usdHydra
    usdLux
    usdMedia
    usdRender
    usdRi
    usdShade
    usdSkel
    usdUI
    usdUtils
    usdVol
    vt
    work
)

set(USD_INCLUDE_DIRS "${USD_INCLUDE_DIRS}" CACHE INTERNAL "USD_INCLUDE_DIRS")
set(USD_LIBRARIES "${USD_LIBRARIES}" CACHE INTERNAL "USD_LIBRARIES")
