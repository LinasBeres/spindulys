#ifndef LIGHT_AREA_H
#define LIGHT_AREA_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/constants.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE


class AreaLight : virtual public Light
{
	public:
		AreaLight(float radiance = 1.f)
			: m_radiance(radiance)
		{ m_flags = (uint32_t) LightFlags::Surface; }

		virtual ~AreaLight() = default;

		// Get Methods
		float GetRadiance() const { return m_radiance; }

		// Set Methods
		bool SetRadiance(float radiance) { return radiance != std::exchange(m_radiance, radiance); }

	protected:
		float m_radiance = 1.f;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_AREA_H
