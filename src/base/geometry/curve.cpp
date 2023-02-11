#include "curve.h"


BASE_NAMESPACE_OPEN_SCOPE

Curve::Curve(Geometry::GeometryTypes type, const std::string& name /* = "DefaultCurve" */)
{
	m_name = name;
	m_geomType = type;
}

BASE_NAMESPACE_CLOSE_SCOPE
