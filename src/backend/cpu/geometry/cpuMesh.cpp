#include "cpuMesh.h"

#include "../utils/interaction.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE

CPUMesh::CPUMesh()
	: Mesh(Geometry::GeometryTypes::Mesh)
{

}

CPUMesh::CPUMesh(Mesh* mesh)
	: Mesh(Geometry::GeometryTypes::Mesh, mesh->GetName())
{
	_transform = mesh->GetTransform();
	_displayColor = mesh->GetDisplayColor();
	_points = mesh->GetPoints();
	_indices = mesh->GetIndices();
	_type = mesh->GetMeshType();
}

bool CPUMesh::CreatePrototype(const RTCDevice& device)
{
	_scene = rtcNewScene(device);
	_geom = rtcNewGeometry(device, _type == MeshType::QuadMesh ? RTC_GEOMETRY_TYPE_QUAD : RTC_GEOMETRY_TYPE_TRIANGLE);
	_geomID = rtcAttachGeometry(_scene, _geom);

	rtcSetSharedGeometryBuffer(_geom,
			RTC_BUFFER_TYPE_VERTEX,
			0,
			RTC_FORMAT_FLOAT3,
			_points.data(),
			0,
			sizeof(Vec3f),
			_points.size());

	switch(_type)
	{
		case MeshType::QuadMesh:
			rtcSetSharedGeometryBuffer(_geom,
					RTC_BUFFER_TYPE_INDEX,
					0,
					RTC_FORMAT_UINT4,
					_indices.data(),
					0,
					sizeof(int) * 4,
					_indices.size() / 4);
				break;
		case MeshType::TriangleMesh:
			rtcSetSharedGeometryBuffer(_geom,
					RTC_BUFFER_TYPE_INDEX,
					0,
					RTC_FORMAT_UINT3,
					_indices.data(),
					0,
					sizeof(int) * 3,
					_indices.size() / 3);
			break;
	}

	return true;
}

SurfaceInteraction CPUMesh::ComputeSurfaceInteraction(const Ray& ray,
		const PreliminaryIntersection& pi,
		uint32_t rayFlags,
		uint32_t recursionDepth, bool active) const
{
	SurfaceInteraction si;

	si.primID = si.primID;
	si.shapeID = _geomID;

	si.t = pi.t;

	const Vec2f primUV = pi.primUV;

	const Vec3i fi = GetTriangleFaceIndex(pi.primIndex);

	const Vec3f p0 = GetVertexPosition(fi[0]);
	const Vec3f p1 = GetVertexPosition(fi[1]);
	const Vec3f p2 = GetVertexPosition(fi[2]);

	const float b1 = primUV.x;
	const float b2 = primUV.y;
	const float b0 = 1.f - b1 - b2;

	const Vec3f dp0 = p1 - p0;
	const Vec3f dp1 = p2 - p0;

	si.p = madd(b0, p0, madd(b1, p1, b2 * p2));

	si.n = normalize(cross(dp0, dp1));

	si.uv = Vec2f(b1, b2);

	if (HasVertexNormals())
	{
		const Vec3f n0 = GetVertexNormal(fi[0]);
		const Vec3f n1 = GetVertexNormal(fi[1]);
		const Vec3f n2 = GetVertexNormal(fi[2]);

		Vec3f n = n2 * b2 + n1 * b1 + n0 * b0;
		const float il = rcp_length(n);
		n *= il;

		si.shadingFrame.vz = n;
	}
	else
	{
		si.shadingFrame.vz = si.n;
	}
	// TODO: Flip normals

	// TODO: Tex coord

	si.shape = this;
	si.instance = nullptr;

	// TODO: Boundry test

	// Currently everything is an instance so...
	ComputeInstanceSurfaceInteraction(si);

	return si;
}


BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
