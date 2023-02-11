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
		const std::vector<Vec3f>& GetPoints()   const { return m_points;  }
		const std::vector<Vec3f>& GetNormals()  const { return m_normals; }
		const std::vector<int>&   GetIndices()  const { return m_indices; }
		MeshType                  GetMeshType() const { return m_type;    }

		// Set Methods
		bool SetPoints(const std::vector<Vec3f> points)   { return points != std::exchange(m_points, points);    }
		bool SetNormals(const std::vector<Vec3f> normals) { return normals != std::exchange(m_normals, normals); }
		bool SetIndices(const std::vector<int> indices)   { return indices != std::exchange(m_indices, indices); }
		bool SetMeshType(MeshType meshType)               { return meshType != std::exchange(m_type, meshType);  }

		bool HasVertexNormals() const { return m_normals.size() != 0; }

	protected:
		std::vector<Vec3f> m_points;
		std::vector<Vec3f> m_normals;
		std::vector<int> m_indices;

		MeshType m_type = MeshType::QuadMesh;

	protected:
		Vec3i GetTriangleFaceIndex(int index) const
		{
			index *= 3;
			return Vec3i(m_indices[index], m_indices[index + 1], m_indices[index + 2]);
		}

		Vec3f GetVertexPosition(int index) const { return m_points[index];  }
		Vec3f GetVertexNormal(int index)   const { return m_normals[index]; }

	private:

};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // QUADMESH_H
