#ifndef LIGHT_POINT_H
#define LIGHT_POINT_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/constants.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE


class PointLight : virtual public Light
{
	public:
		PointLight(const AffineSpace3f& transform = AffineSpace3f(one, zero), float intensity = 1.f)
			: Light(transform), m_intensity(intensity)
		{ m_flags = (uint32_t) LightFlags::DeltaPosition; }

		virtual ~PointLight() = default;

		// Get Methods
		float        GetIntensity() const { return m_intensity;      }
		const Vec3f& GetPosition()  const { return GetTransform().p; }

		// Set Methods
		bool SetIntensity(float intensity )     { return intensity != std::exchange(m_intensity, intensity);  }
		bool SetPosition(const Vec3f& position) { return position  != std::exchange(m_transform.p, position); }

	protected:
		float m_intensity = 1.f;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_POINT_H
