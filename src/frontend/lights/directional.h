#ifndef LIGHT_DIRECTIONAL_H
#define LIGHT_DIRECTIONAL_H

#include <spindulys/math/col3.h>
#include <spindulys/math/constants.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE


class DirectionalLight : virtual public Light
{
	public:
		DirectionalLight(const Col3f& irradiance = Col3f(one))
			: m_irradiance(irradiance)
		{ m_flags = (uint32_t) LightFlags::DeltaDirection | (uint32_t) LightFlags::Infinite; }

		virtual ~DirectionalLight() = default;

		// Get Methods
		const Col3f& GetIrradiance() const { return m_irradiance; }

		// Set Methods
		bool SetIrradiance(const Col3f& irradiance) { return irradiance != std::exchange(m_irradiance, irradiance); }

	protected:
		Col3f m_irradiance = Col3f(one);

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_DIRECTIONAL_H
