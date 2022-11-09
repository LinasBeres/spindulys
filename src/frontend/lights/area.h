#ifndef LIGHT_AREA_H
#define LIGHT_AREA_H

#include <spindulys/math/col3.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class AreaLight : virtual public Light
{
	public:
		AreaLight(const Col3f& radiance = Col3f(one))
			: m_radiance(radiance)
		{ m_flags = (uint32_t) LightFlags::Surface; }

		virtual ~AreaLight() = default;

		// Get Methods
		const Col3f& GetRadiance() const { return m_radiance; }

		// Set Methods
		bool SetRadiance(const Col3f& radiance) { return radiance != std::exchange(m_radiance, radiance); }

	protected:
		Col3f m_radiance = Col3f(one);

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_AREA_H
