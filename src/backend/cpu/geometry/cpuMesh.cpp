#include "cpuMesh.h"


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


BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
