#ifndef RENDER_GLOBALS_H
#define RENDER_GLOBALS_H

#include <unordered_set>

#include <spindulys/buffer.h>
#include <spindulys/defaults.h>

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

enum class SamplerIds
{
	kIndependent = 0,
	kUniform,
	kStratified,
};

enum class IntegratorIds : uint32_t
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
	kDirect,
	kIndirect,
};

struct RenderGlobals
{
	// Resolution
	uint32_t m_width = kDefaultWidth;
	uint32_t m_height = kDefaultHeight;

	// General Render
	uint32_t m_maxIterations = kDefaultMaxIterations;
	uint32_t m_maxSamplesPerPixel = kDefaultMaxSamplesPerPixel;

	// Integrator
	IntegratorIds m_integratorID = IntegratorIds::kForwardPath;
	bool m_hideLights = kHideLights;

	// Direct Integrator
	uint32_t m_maxLightSamples = kMaxLightSamples;
	uint32_t m_maxBSDFSamples = kMaxBSDFSamples;

	// Forward Path Integrator
	uint32_t m_maxDepth = kDefaultMaxDepth;
	uint32_t m_russianRouletteDepth = kDefaultRussianRouletteDepth;

	// Samplers - sampling method to use.
	SamplerIds m_samplerId = SamplerIds::kIndependent;

	// Buffers
	BufferIds m_bufferID = BufferIds::kBeauty;
	std::unordered_set<BufferIds> m_currentBufferIds = { BufferIds::kBeauty };

	// Resolution Scaling
	bool m_scaleResolution = kDefaultScaleResolution;
	float m_growSize = kDefaultGrowSize;

	// ----------------------- Set Methods -----------------------
	// Return true if the class parameter was changed.
	bool SetWidth(uint32_t width)                  { return width         != std::exchange(m_width, width);                       }
	bool SetHeight(uint32_t height)                { return height        != std::exchange(m_height, height);                     }

	bool SetMaxIterations(uint32_t maxIterations)  { return maxIterations != std::exchange(m_maxIterations, maxIterations);       }
	bool SetMaxSamples(uint32_t samples)           { return samples       != std::exchange(m_maxSamplesPerPixel, samples);        }

	bool SetIntegrator(IntegratorIds integratorID) { return integratorID  != std::exchange(m_integratorID, integratorID);         }
	bool SetHideLights(bool hideLights)            { return hideLights    != std::exchange(m_hideLights, hideLights);             }

	bool SetMaxLightSamples(uint32_t maxLightSamples)
	{
		return maxLightSamples != std::exchange(m_maxLightSamples, maxLightSamples) && m_integratorID == IntegratorIds::kDirect;
	}
	bool SetMaxBSDFSamples(uint32_t maxBSDFSamples)
	{
		return maxBSDFSamples != std::exchange(m_maxBSDFSamples, maxBSDFSamples) && m_integratorID == IntegratorIds::kDirect;
	}

	bool SetMaxDepth(uint32_t maxDepth)
	{
		return maxDepth != std::exchange(m_maxDepth, maxDepth) && m_integratorID == IntegratorIds::kForwardPath;
	}
	bool SetRussianRouletteDepth(uint32_t rrDepth)
	{
		return rrDepth != std::exchange(m_russianRouletteDepth, rrDepth) && m_integratorID == IntegratorIds::kForwardPath;
	}

	bool SetSampler(SamplerIds samplerId) { return samplerId != std::exchange(m_samplerId, samplerId); }

	bool SetCurrentBuffer(BufferIds bufferID) { return bufferID != std::exchange(m_bufferID, bufferID); }
	bool AddBuffer(BufferIds bufferID)
	{
		if (m_currentBufferIds.find(bufferID) != m_currentBufferIds.end())
			return false;

		m_currentBufferIds.emplace(bufferID);
		return true;
	}
	bool RemoveBuffer(BufferIds bufferID)
	{
	// Beauty cannot be removed
	if (bufferID == BufferIds::kBeauty)
		return false;

	// Current buffer already exists so do nothing and signify that no update needs to happen.
	if (m_currentBufferIds.find(bufferID) != m_currentBufferIds.end())
		return false;

	return true;
	}


	bool SetScaleResolution(bool scaleResolution)  { return scaleResolution != std::exchange(m_scaleResolution, scaleResolution);        }
	bool SetGrowSize(float growSize)               { return growSize        != std::exchange(m_growSize, growSize) && m_scaleResolution; }

	// ----------------------- Get Methods -----------------------
	uint32_t                             GetWidth()                const { return m_width;                }
	uint32_t                             GetHeight()               const { return m_height;               }

	uint32_t                             GetMaxIterations()        const { return m_maxIterations;        }
	uint32_t                             GetMaxSamples()           const { return m_maxSamplesPerPixel;   }

	IntegratorIds                        GetIntegrator()           const { return m_integratorID;         }
	bool                                 GetHideLights()           const { return m_hideLights;           }

	uint32_t                             GetMaxLightsSamples()     const { return m_maxLightSamples;      }
	uint32_t                             GetMaxBSDFSamples()       const { return m_maxBSDFSamples;       }

	uint32_t                             GetMaxDepth()             const { return m_maxDepth;             }
	uint32_t                             GetRussianRouletteDepth() const { return m_russianRouletteDepth; }

	SamplerIds                           GetSampler()              const { return m_samplerId;            }

	BufferIds                            GetBufferID()             const { return m_bufferID;             }
	const std::unordered_set<BufferIds>& GetCurrentBufferIds()     const { return m_currentBufferIds;     }

	bool                                 GetScaleResolution()      const { return m_scaleResolution;      }
	float                                GetGrowSize()             const { return m_growSize;             }
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // RENDER_GLOBALS_H
