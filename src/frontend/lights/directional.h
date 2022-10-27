#ifndef LIGHT_DIRECTIONAL_H
#define LIGHT_DIRECTIONAL_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/constants.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE


class DirectionalLight : virtual public Light
{
	public:
		DirectionalLight(float irradiance = 1.f)
			: m_irradiance(irradiance)
		{ m_flags = (uint32_t) LightFlags::DeltaDirection | (uint32_t) LightFlags::Infinite; }

		virtual ~DirectionalLight() = default;

		// Get Methods
		float GetIrradiance() const { return m_irradiance; }

		// Set Methods
		bool SetIrradiance(float irradiance) { return irradiance != std::exchange(m_irradiance, irradiance); }

	protected:
		float m_irradiance = 1.f;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_DIRECTIONAL_H
