#include "cpuCurve.h"

#include <spindulys/math/vec4.h>

#include "../utils/interaction.h"


CPU_NAMESPACE_OPEN_SCOPE

CPUCurve::CPUCurve()
	: Curve(Geometry::GeometryTypes::Curve)
{

}

CPUCurve::CPUCurve(Curve* curve)
	: Curve(Geometry::GeometryTypes::Mesh, curve->GetName())
{
	m_transform = curve->GetTransform();
	m_displayColor = curve->GetDisplayColor();
	m_points = curve->GetPoints();
	m_normals = curve->GetNormals();
	m_widths = curve->GetWidths();
	m_curveType = curve->GetCurveType();
}

bool CPUCurve::CreatePrototype(const RTCDevice& device)
{
	m_scene = rtcNewScene(device);

	switch(m_curveType)
	{
		case CurveTypes::FlatLinear:
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_FLAT_LINEAR_CURVE);
			break;
		case CurveTypes::FlatBezier:
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_FLAT_BEZIER_CURVE);
			break;
		case CurveTypes::RoundBezier:
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_ROUND_BEZIER_CURVE);
			break;
		case CurveTypes::NormalOrientatedBezier:
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_NORMAL_ORIENTED_BEZIER_CURVE);
			break;
		case CurveTypes::FlatBSpline:
			// TODO: Find a test scene for this.
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_FLAT_BSPLINE_CURVE);
			break;
		case CurveTypes::RoundBSpline:
			// TODO: Find a test scene for ths.
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_ROUND_BSPLINE_CURVE);
			break;
		case CurveTypes::NormalOrientatedBSpline:
			// TODO: Find a test scene for ths.
			m_geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_NORMAL_ORIENTED_BSPLINE_CURVE);
			break;
	}
	m_geomID = rtcAttachGeometry(m_scene, m_geom);

	rtcSetGeometryVertexAttributeCount(m_geom, 1);

	if (m_curveType == CurveTypes::FlatLinear)
	{
		unsigned int* indices = (unsigned int*)rtcSetNewGeometryBuffer(
				m_geom,
				RTC_BUFFER_TYPE_INDEX,
				0,
				RTC_FORMAT_UINT,
				sizeof(unsigned int),
				m_points.size());
		for (unsigned int i = 0; i < static_cast<unsigned int>(m_points.size()); ++i)
			indices[i] = i;
	}
	else
	{
		unsigned int* indices = (unsigned int*)rtcSetNewGeometryBuffer(
				m_geom,
				RTC_BUFFER_TYPE_INDEX,
				0,
				RTC_FORMAT_UINT,
				sizeof(unsigned int),
				(m_points.size() % 3) * 3);
		for (unsigned int i = 0; i < static_cast<unsigned int>((m_points.size() % 3) * 3); ++i)
		{
			indices[i] = i * 3;
		}
	}

	Vec4f* verts = (Vec4f*)rtcSetNewGeometryBuffer(m_geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(Vec4f), m_points.size());
	for (int i = 0; i < static_cast<int>(m_points.size()); ++i) {
    verts[i] = Vec4f(m_points[i].x, m_points[i].y, m_points[i].z, m_widths[i] / 2.f);
  }

	if (m_curveType == CurveTypes::NormalOrientatedBezier || m_curveType == CurveTypes::NormalOrientatedBSpline)
	{
		rtcSetSharedGeometryBuffer(m_geom,
				RTC_BUFFER_TYPE_NORMAL,
				0,
				RTC_FORMAT_FLOAT3,
				m_normals.data(),
				0,
				sizeof(Vec3f),
				m_normals.size());
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
	si.shapeID = m_geomID;

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


CPU_NAMESPACE_CLOSE_SCOPE
