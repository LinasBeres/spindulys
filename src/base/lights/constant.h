#ifndef LIGHT_CONSTANT_H
#define LIGHT_CONSTANT_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/col3.h>
#include <spindulys/math/constants.h>

#include "../spindulysBase.h"

#include "light.h"


BASE_NAMESPACE_OPEN_SCOPE


class ConstantLight : virtual public Light
{
	public:
		ConstantLight(float radius = 1.f, const Vec3f& point = Vec3f(zero),
				float surfaceArea = 1.f, const Col3f& radiance = Col3f(one))
			: m_radius(radius), m_point(point), m_surfaceArea(surfaceArea), m_radiance(radiance)
		{ m_flags = (uint32_t) LightFlags::Infinite; }

		virtual ~ConstantLight() = default;

		// Get Methods
		float        GetRadius()      const { return m_radius;      }
		const Vec3f& GetPoint()       const { return m_point;       }
		float        GetSurfaceArea() const { return m_surfaceArea; }
		const Col3f& GetRadiance()    const { return m_radiance;    }

		// Set Methods
		bool SetRadius(float radius)            { return radius      != std::exchange(m_radius, radius);           }
		bool SetPoint(const Vec3f& point)       { return point       != std::exchange(m_point, point);             }
		bool SetSurfaceArea(float surfaceArea)  { return surfaceArea != std::exchange(m_surfaceArea, surfaceArea); }
		bool SetRadiance(const Col3f& radiance) { return radiance    != std::exchange(m_radiance, radiance);       }

	protected:
		float m_radius      = 1.f;
		Vec3f m_point       = Vec3f(zero);

		float m_surfaceArea = FourPi<float>;

		Col3f m_radiance    = Col3f(one);

	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_H
