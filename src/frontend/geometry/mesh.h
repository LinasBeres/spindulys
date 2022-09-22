#ifndef MESH_H
#define MESH_H

#include <vector>

#include "../spindulysFrontend.h"

#include "geometry.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class Mesh : virtual public Geometry
{
	public:
		enum MeshType
		{
			TriangleMesh = 0,
			QuadMesh,
		};

		Mesh(Geometry::GeometryTypes type, const std::string& name = "DefaultMesh");

		// Get Methods
		const std::vector<Vec3f>& GetPoints()   const { return _points;  }
		const std::vector<Vec3f>& GetNormals()  const { return _normals; }
		const std::vector<int>&   GetIndices()  const { return _indices; }
		MeshType                  GetMeshType() const { return _type;    }

		// Set Methods
		bool SetPoints(const std::vector<Vec3f> points)   { return points != std::exchange(_points, points);    }
		bool SetNormals(const std::vector<Vec3f> normals) { return normals != std::exchange(_normals, normals); }
		bool SetIndices(const std::vector<int> indices)   { return indices != std::exchange(_indices, indices); }
		bool SetMeshType(MeshType meshType)               { return meshType != std::exchange(_type, meshType);  }

	protected:
		std::vector<Vec3f> _points;
		std::vector<Vec3f> _normals;
		std::vector<int> _indices;

		MeshType _type = MeshType::QuadMesh;
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // QUADMESH_H
