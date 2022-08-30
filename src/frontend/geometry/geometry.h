#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/col3.h>
#include <spindulys/math/affinespace.h>

#include "../spindulysFrontend.h"

#include <pxr/base/vt/array.h>


FRONTEND_NAMESPACE_OPEN_SCOPE

class Geometry
{
	public:
		enum GeometryTypes
		{
			QuadMesh = 0,
			TriangleMesh,
			Mesh,
		};

		Geometry() = default;
		virtual ~Geometry();

		// Get methods
		const std::string&  GetName()           const { return _name;           }
		const Col3f&        GetDisplayColor()   const { return _displayColor;   }
		const AffineSpace3f GetTransform()      const { return _transform;      }
		unsigned int        GetGeomID()         const { return _geomID;         }
		unsigned int        GetGeomInstanceID() const { return _geomInstanceID; }
		GeometryTypes       GetGeometryType()   const { return _geomType;       }

		// Set Methods
		bool SetName(const std::string& name)           { return name         != std::exchange(_name, name);                 }
		bool SetTransfrom(const AffineSpace3f& affine)  { return affine       != std::exchange(_transform, affine);          }
		bool SetDisplayColor(const Col3f& displayColor) { return displayColor != std::exchange(_displayColor, displayColor); }
		bool SetGeometryType(GeometryTypes type)        { return type         != std::exchange(_geomType, type);             }

	protected:
		unsigned int _geomID         = SPINDULYS_INVALID_GEOMETRY_ID;
		unsigned int _geomInstanceID = SPINDULYS_INVALID_GEOMETRY_ID;

		std::string _name = "DefaultGeomerty";
		AffineSpace3f _transform = AffineSpace3f(one, zero);
		Col3f _displayColor = Col3f(0.5);
		GeometryTypes _geomType;
	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // GEOMETRY_H
