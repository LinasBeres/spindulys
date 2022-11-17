#ifndef LIGHT_POINT_H
#define LIGHT_POINT_H

#include <spindulys/math/col3.h>
#include <spindulys/math/constants.h>

#include "../spindulysBase.h"

#include "light.h"


BASE_NAMESPACE_OPEN_SCOPE


class PointLight : virtual public Light
{
	public:
		PointLight(const AffineSpace3f& transform = AffineSpace3f(one, zero), const Col3f& intensity = one)
			: m_intensity(intensity)
		{
			m_transform = transform;
			m_flags = (uint32_t) LightFlags::DeltaPosition;
		}

		virtual ~PointLight() = default;

		// Get Methods
		const Col3f& GetIntensity() const { return m_intensity;      }
		const Vec3f& GetPosition()  const { return GetTransform().p; }

		// Set Methods
		bool SetIntensity(const Col3f& intensity) { return intensity != std::exchange(m_intensity, intensity);  }
		bool SetPosition(const Vec3f& position)   { return position  != std::exchange(m_transform.p, position); }

	protected:
		Col3f m_intensity = one;

	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_POINT_H
