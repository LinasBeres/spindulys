#ifndef RENDER_GLOBALS_H
#define RENDER_GLOBALS_H

#include <unordered_set>

#include <spindulys/sampler.h>
#include <spindulys/buffer.h>
#include <spindulys/defaults.h>

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

enum class IntegratorIds
{
	kDirect = 0,
	kForwardPath,
};

enum class BufferIds
{
	kBeauty = 0,
	kDiffuse,
	kPosition,
	kNormal,
	kDebug,
};

struct RenderGlobals
{
	// Resolution
	uint32_t width = kDefaultWidth;
	uint32_t height = kDefaultHeight;

	// General Render
	uint32_t maxIterations = kDefaultMaxIterations;
	uint32_t maxSamplesPerPixel = kDefaultMaxSamplesPerPixel;

	// Integrator
	IntegratorIds integratorID = IntegratorIds::kForwardPath;
	bool hideLights = kHideLights;

	// Direct Integrator
	size_t maxLightSamples = kMaxLightSamples;
	size_t maxBSDFSamples = kMaxBSDFSamples;

	// Forward Path Integrator
	uint32_t maxDepth = kDefaultMaxDepth;
	uint32_t russianRouletteDepth = kDefaultRussianRouletteDepth;

	// Buffers
	BufferIds bufferID = BufferIds::kBeauty;
	std::unordered_set<BufferIds> currentBufferIds = { BufferIds::kBeauty, BufferIds::kDiffuse, BufferIds::kPosition, BufferIds::kNormal, BufferIds::kDebug };

	// Resolution Scaling
	bool scaleResolution = kDefaultScaleResolution;
	float growSize = kDefaultGrowSize;
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_GLOBALS_H
