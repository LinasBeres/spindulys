#ifndef SPINDULYS_DEFAULTS_H
#define SPINDULYS_DEFAULTS_H

#include "../spindulys.h"

SPINDULYS_NAMESPACE_OPEN_SCOPE

// Render Globals
static constexpr uint32_t kDefaultWidth = 600;
static constexpr uint32_t kDefaultHeight = 300;

static constexpr uint32_t kDefaultMaxIterations = 32;
static constexpr uint32_t kDefaultMaxSamplesPerPixel = 1;

static constexpr bool kHideLights = false;

static constexpr size_t kMaxLightSamples = 1;
static constexpr size_t kMaxBSDFSamples = 1;

static constexpr uint32_t kDefaultMaxDepth = 2;
static constexpr uint32_t kDefaultRussianRouletteDepth = 5;

static constexpr bool kDefaultScaleResolution = false;
static constexpr float kDefaultGrowSize = 0.25f;


SPINDULYS_NAMESPACE_CLOSE_SCOPE

#endif // SPINDULYS_DEFAULTS_H
