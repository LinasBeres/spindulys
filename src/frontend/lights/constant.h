#ifndef LIGHT_CONSTANT_H
#define LIGHT_CONSTANT_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/constants.h>

#include "../spindulysFrontend.h"

#include "light.h"


FRONTEND_NAMESPACE_OPEN_SCOPE


class ConstantLight : virtual public Light
{
	public:
		ConstantLight(float radius = 1.f, const Vec3f& point = Vec3f(zero),
				float surfaceArea = 1.f, float radiance = 1.f)
			: m_radius(radius), m_point(point), m_surfaceArea(surfaceArea), m_radiance(radiance)
		{ m_flags = (uint32_t) LightFlags::Infinite; }

		virtual ~ConstantLight() = default;

		// Get Methods
		float        GetRadius()      const { return m_radius;      }
		const Vec3f& GetPoint()       const { return m_point;       }
		float        GetSurfaceArea() const { return m_surfaceArea; }
		float        GetRadiance()    const { return m_radiance;    }

		// Set Methods
		bool SetRadius(float radius)           { return radius      != std::exchange(m_radius, radius);           }
		bool SetPoint(const Vec3f& point)      { return point       != std::exchange(m_point, point);             }
		bool SetSurfaceArea(float surfaceArea) { return surfaceArea != std::exchange(m_surfaceArea, surfaceArea); }
		bool SetRadiance(float radiance)       { return radiance    != std::exchange(m_radiance, radiance);       }

	protected:
		float m_radius      = 1.f;
		Vec3f m_point       = Vec3f(zero);

		float m_surfaceArea = FourPi<float>;

		float m_radiance    = 1.f;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_H
