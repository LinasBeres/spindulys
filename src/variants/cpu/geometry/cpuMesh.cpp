#include "cpuMesh.h"

#include "../utils/interaction.h"

CPU_NAMESPACE_OPEN_SCOPE

CPUMesh::CPUMesh()
	: Mesh(Geometry::GeometryTypes::Mesh)
{

}

CPUMesh::CPUMesh(Mesh* mesh)
	: Mesh(Geometry::GeometryTypes::Mesh, mesh->GetName())
{
	m_transform = mesh->GetTransform();
	m_displayColor = mesh->GetDisplayColor();
	m_points = mesh->GetPoints();
	m_indices = mesh->GetIndices();
	m_type = mesh->GetMeshType();
}

bool CPUMesh::CreatePrototype(const RTCDevice& device)
{
	m_scene = rtcNewScene(device);
	m_geom = rtcNewGeometry(device, m_type == MeshType::QuadMesh ? RTC_GEOMETRY_TYPE_QUAD : RTC_GEOMETRY_TYPE_TRIANGLE);
	m_geomID = rtcAttachGeometry(m_scene, m_geom);

	rtcSetSharedGeometryBuffer(m_geom,
			RTC_BUFFER_TYPE_VERTEX,
			0,
			RTC_FORMAT_FLOAT3,
			m_points.data(),
			0,
			sizeof(Vec3f),
			m_points.size());

	switch(m_type)
	{
		case MeshType::QuadMesh:
			rtcSetSharedGeometryBuffer(m_geom,
					RTC_BUFFER_TYPE_INDEX,
					0,
					RTC_FORMAT_UINT4,
					m_indices.data(),
					0,
					sizeof(int) * 4,
					m_indices.size() / 4);
				break;
		case MeshType::TriangleMesh:
			rtcSetSharedGeometryBuffer(m_geom,
					RTC_BUFFER_TYPE_INDEX,
					0,
					RTC_FORMAT_UINT3,
					m_indices.data(),
					0,
					sizeof(int) * 3,
					m_indices.size() / 3);
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
	si.shapeID = m_geomID;

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

	si.n = cross(dp0, dp1);
	si.n = normalize(si.n);

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

	if (/* flip normals */ false)
	{
		si.n = -si.n;
		si.shadingFrame.vz = -si.shadingFrame.vz;
	}

	// TODO: Tex coord

	si.shape = this;
	si.instance = nullptr;

	// TODO: Boundry test

	// TODO: Currently everything is an instance so...
	ComputeInstanceSurfaceInteraction(si, ray);


	return si;
}


CPU_NAMESPACE_CLOSE_SCOPE
