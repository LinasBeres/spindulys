#include "cpuCurve.h"

#include <spindulys/math/vec4.h>

#include "../utils/interaction.h"


BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUCurve::CPUCurve()
	: Curve(Geometry::GeometryTypes::Curve)
{

}

CPUCurve::CPUCurve(Curve* curve)
	: Curve(Geometry::GeometryTypes::Mesh, curve->GetName())
{
	_transform = curve->GetTransform();
	_displayColor = curve->GetDisplayColor();
	_points = curve->GetPoints();
	_normals = curve->GetNormals();
	_widths = curve->GetWidths();
	_curveType = curve->GetCurveType();
}

bool CPUCurve::CreatePrototype(const RTCDevice& device)
{
	_scene = rtcNewScene(device);

	switch(_curveType)
	{
		case CurveTypes::FlatLinear:
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_FLAT_LINEAR_CURVE);
			break;
		case CurveTypes::FlatBezier:
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_FLAT_BEZIER_CURVE);
			break;
		case CurveTypes::RoundBezier:
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_ROUND_BEZIER_CURVE);
			break;
		case CurveTypes::NormalOrientatedBezier:
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_NORMAL_ORIENTED_BEZIER_CURVE);
			break;
		case CurveTypes::FlatBSpline:
			// TODO: Find a test scene for this.
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_FLAT_BSPLINE_CURVE);
			break;
		case CurveTypes::RoundBSpline:
			// TODO: Find a test scene for ths.
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_ROUND_BSPLINE_CURVE);
			break;
		case CurveTypes::NormalOrientatedBSpline:
			// TODO: Find a test scene for ths.
			_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_NORMAL_ORIENTED_BSPLINE_CURVE);
			break;
	}
	_geomID = rtcAttachGeometry(_scene, _geom);

	rtcSetGeometryVertexAttributeCount(_geom, 1);

	if (_curveType == CurveTypes::FlatLinear)
	{
		unsigned int* indices = (unsigned int*)rtcSetNewGeometryBuffer(
				_geom,
				RTC_BUFFER_TYPE_INDEX,
				0,
				RTC_FORMAT_UINT,
				sizeof(unsigned int),
				_points.size());
		for (unsigned int i = 0; i < static_cast<unsigned int>(_points.size()); ++i)
			indices[i] = i;
	}
	else
	{
		unsigned int* indices = (unsigned int*)rtcSetNewGeometryBuffer(
				_geom,
				RTC_BUFFER_TYPE_INDEX,
				0,
				RTC_FORMAT_UINT,
				sizeof(unsigned int),
				(_points.size() % 3) * 3);
		for (unsigned int i = 0; i < static_cast<unsigned int>((_points.size() % 3) * 3); ++i)
		{
			indices[i] = i * 3;
		}
	}

	Vec4f* verts = (Vec4f*)rtcSetNewGeometryBuffer(_geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(Vec4f), _points.size());
	for (int i = 0; i < static_cast<int>(_points.size()); ++i) {
    verts[i] = Vec4f(_points[i].x, _points[i].y, _points[i].z, _widths[i] / 2.f);
  }

	if (_curveType == CurveTypes::NormalOrientatedBezier || _curveType == CurveTypes::NormalOrientatedBSpline)
	{
		rtcSetSharedGeometryBuffer(_geom,
				RTC_BUFFER_TYPE_NORMAL,
				0,
				RTC_FORMAT_FLOAT3,
				_normals.data(),
				0,
				sizeof(Vec3f),
				_normals.size());
	}

	return true;
}

SurfaceInteraction CPUCurve::ComputeSurfaceInteraction(const Ray& ray,
		const PreliminaryIntersection& pi,
		uint32_t rayFlags,
		uint32_t recursionDepth, bool active) const
{
	SurfaceInteraction si;

	si.primID = si.primID;
	si.shapeID = _geomID;

	si.t = pi.t;
	si.p = ray.origin + ray.tfar * ray.direction;

	// TODO: Use the normals primvar if available (for smooth normals).
	si.n = normalize(ray.Ng);
	si.shadingFrame.vz = si.n;

	si.shape = this;
	si.instance = nullptr;

	si.uv = pi.primUV;

	// Currently everything is an instance so...
	ComputeInstanceSurfaceInteraction(si, ray);

	return si;
}


BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
