#include "mesh.h"


BASE_NAMESPACE_OPEN_SCOPE

Mesh::Mesh(Geometry::GeometryTypes type, const std::string& name /* = "DefaultMesh" */)
{
	m_name = name;
	m_geomType = type;
}

BASE_NAMESPACE_CLOSE_SCOPE
