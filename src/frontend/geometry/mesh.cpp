#include "mesh.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

Mesh::Mesh(Geometry::GeometryTypes type, const std::string& name /* = "DefaultMesh" */)
{
	_name = name;
	_geomType = type;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
