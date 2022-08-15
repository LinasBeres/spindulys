#ifndef TRIANGLEMESH_H
#define TRIANGLEMESH_H

#include "../spindulysFrontend.h"

#include "geometry.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class TriangleMesh : virtual public Geometry
{
	public:
		TriangleMesh();
		TriangleMesh(const std::string& primName,
				const LinearSpace3f& transform,
				const Col3f& displayColor,
				const pxr::VtArray<pxr::GfVec3f>& points,
				const pxr::VtArray<int>& indices);

	protected:
		pxr::VtArray<pxr::GfVec3f> _points;
		pxr::VtArray<int> _indices;
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // TRIANGLEMESH_H
