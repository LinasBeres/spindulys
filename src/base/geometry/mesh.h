#ifndef MESH_H
#define MESH_H

#include <vector>

#include "../spindulysBase.h"

#include "geometry.h"


BASE_NAMESPACE_OPEN_SCOPE

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

		bool HasVertexNormals() const { return _normals.size() != 0; }

	protected:
		std::vector<Vec3f> _points;
		std::vector<Vec3f> _normals;
		std::vector<int> _indices;

		MeshType _type = MeshType::QuadMesh;

	protected:
		Vec3i GetTriangleFaceIndex(int index) const
		{
			index *= 3;
			return Vec3i(_indices[index], _indices[index + 1], _indices[index + 2]);
		}

		Vec3f GetVertexPosition(int index) const { return _points[index];  }
		Vec3f GetVertexNormal(int index)   const { return _normals[index]; }

	private:

};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // QUADMESH_H
