#ifndef LIGHT_DIRECTIONAL_AREA_H
#define LIGHT_DIRECTIONAL_AREA_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/constants.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE


class DirectionalAreaLight : virtual public Light
{
	public:
		DirectionalAreaLight(float radiance = 1.f, float area = 0.f)
			: m_radiance(radiance), m_area(area)
		{ m_flags = (uint32_t) LightFlags::Surface | (uint32_t) LightFlags::DeltaDirection; }

		virtual ~DirectionalAreaLight() = default;

		// Get Methods
		float GetRadiance() const { return m_radiance; }
		float GetArea()     const { return m_area;     }

		// Set Methods
		bool SetRadiance(float radiance) { return radiance != std::exchange(m_radiance, radiance); }
		bool SetArea(float area)         { return area != std::exchange(m_area, area);             }

	protected:
		float m_radiance = 1.f;
		float m_area     = 0.f;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_DIRECTIONAL_AREA_H
