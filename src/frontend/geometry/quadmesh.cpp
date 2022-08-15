#include "quadmesh.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

QuadMesh::QuadMesh()
{
}

QuadMesh::QuadMesh(const std::string& primName,
		const LinearSpace3f& transform,
		const Col3f& displayColor,
		const pxr::VtArray<pxr::GfVec3f>& points,
		const pxr::VtArray<int>& indices)
{
	_points = points;
	_indices = indices;

	_name = primName;

	_transform = transform;

	_displayColor = displayColor;
}


FRONTEND_NAMESPACE_CLOSE_SCOPE
