#include "mesh.h"


BASE_NAMESPACE_OPEN_SCOPE

Mesh::Mesh(Geometry::GeometryTypes type, const std::string& name /* = "DefaultMesh" */)
{
	_name = name;
	_geomType = type;
}

BASE_NAMESPACE_CLOSE_SCOPE
