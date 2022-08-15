#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <spindulys/math/vec3.h>
#include <spindulys/math/col3.h>
#include <spindulys/math/linearspace3.h>

#include "../spindulysFrontend.h"

#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/xformCache.h>
#include <pxr/usd/usd/attribute.h>
#include <pxr/base/vt/array.h>

FRONTEND_NAMESPACE_OPEN_SCOPE

class Geometry
{
	public:
		enum GeometryTypes
		{
			QuadMesh = 0,
			TriangleMesh,
		};

		Geometry();
		virtual ~Geometry();

		const std::string& GetName() const { return _name; }
		const Col3f& GetDisplayColor() const { return _displayColor; }
		const LinearSpace3f GetTransform() const { return _transform; }

		unsigned int GetGeomID() const { return _geomID; }
		unsigned int GetGeomInstanceID() const { return _geomInstanceID; }

	protected:
		unsigned int _geomID         = SPINDULYS_INVALID_GEOMETRY_ID;
		unsigned int _geomInstanceID = SPINDULYS_INVALID_GEOMETRY_ID;

		std::string _name;
		LinearSpace3f _transform;
		Col3f _displayColor;
	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // GEOMETRY_H
