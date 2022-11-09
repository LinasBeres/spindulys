#include "curve.h"


BASE_NAMESPACE_OPEN_SCOPE

Curve::Curve(Geometry::GeometryTypes type, const std::string& name /* = "DefaultCurve" */)
{
	_name = name;
	_geomType = type;
}

BASE_NAMESPACE_CLOSE_SCOPE
