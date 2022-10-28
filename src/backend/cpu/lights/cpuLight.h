#ifndef CPU_LIGHT_H
#define CPU_LIGHT_H

#include <spindulys/math/col3.h>

#include <lights/light.h>

#include "../spindulysBackendCPU.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

struct Interaction;
struct SurfaceInteraction;
struct DirectionSample;

class CPULight : virtual public Light
{
	public:
		CPULight() = default;
		CPULight(const AffineSpace3f& transform) : Light(transform) { }

		virtual ~CPULight() = default;

		virtual Col3f Eval(const SurfaceInteraction& si, uint32_t active) const = 0;

		virtual std::pair<DirectionSample, Col3f> SampleDirection(
				const Interaction& it, const Vec2f& sample,
				uint32_t active) const = 0;

		virtual float PdfDirection(const Interaction& it, const DirectionSample& ds,
				uint32_t active) const = 0;

	protected:

	private:
};

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE

#endif // CPU_LIGHT_H
