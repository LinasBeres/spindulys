#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <utility>

#include <spindulys/math/vec3.h>
#include <spindulys/math/col3.h>
#include <spindulys/math/affinespace.h>

#include "../lights/light.h"

#include "../spindulysBase.h"


BASE_NAMESPACE_OPEN_SCOPE

class Geometry
{
	public:
		enum GeometryTypes
		{
			QuadMesh = 0,
			TriangleMesh,
			Mesh,
			Curve,
		};

		Geometry() = default;
		virtual ~Geometry();

		// Get methods
		const std::string&  GetName()           const { return m_name;           }
		const Col3f&        GetDisplayColor()   const { return m_displayColor;   }
		const AffineSpace3f GetTransform()      const { return m_transform;      }
		unsigned int        GetGeomID()         const { return m_geomID;         }
		unsigned int        GetGeomInstanceID() const { return m_geomInstanceID; }
		GeometryTypes       GetGeometryType()   const { return m_geomType;       }
		const Light*        GetLight()          const { return m_light.get();   }

		// Set Methods
		bool SetName(const std::string& name)           { return name         != std::exchange(m_name, name);                 }
		bool SetTransfrom(const AffineSpace3f& affine)  { return affine       != std::exchange(m_transform, affine);          }
		bool SetDisplayColor(const Col3f& displayColor) { return displayColor != std::exchange(m_displayColor, displayColor); }
		bool SetGeometryType(GeometryTypes type)        { return type         != std::exchange(m_geomType, type);             }

		bool IsLight() const { return (bool) m_light; }


	protected:
		unsigned int m_geomID         = SPINDULYS_INVALID_GEOMETRY_ID;
		unsigned int m_geomInstanceID = SPINDULYS_INVALID_GEOMETRY_ID;

		std::string m_name = "DefaultGeomerty";
		AffineSpace3f m_transform = AffineSpace3f(one, zero);
		Col3f m_displayColor = Col3f(0.5);
		GeometryTypes m_geomType;

		std::unique_ptr<Light> m_light = nullptr;
	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // GEOMETRY_H
