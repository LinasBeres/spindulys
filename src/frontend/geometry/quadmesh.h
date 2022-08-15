#ifndef QUADMESH_H
#define QUADMESH_H

#include "../spindulysFrontend.h"

#include "geometry.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class QuadMesh : virtual public Geometry
{
	public:
		QuadMesh();
		QuadMesh(const std::string& primName,
				const LinearSpace3f& transform,
				const Col3f& displayColor,
				const pxr::VtArray<pxr::GfVec3f>& points,
				const pxr::VtArray<int>& indices);

	protected:
		pxr::VtArray<pxr::GfVec3f> _points;
		pxr::VtArray<int> _indices;
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // QUADMESH_H
