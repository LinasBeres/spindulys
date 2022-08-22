#include "trianglemesh.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

TriangleMesh::TriangleMesh()
{
}

TriangleMesh::TriangleMesh(const std::string& primName,
		const AffineSpace3f& affine,
		const Col3f& displayColor,
		const pxr::VtArray<pxr::GfVec3f>& points,
		const pxr::VtArray<int>& indices)
{
	_points = points;
	_indices = indices;

	_name = primName;

	_transform = affine;

	_displayColor = displayColor;
}


FRONTEND_NAMESPACE_CLOSE_SCOPE
