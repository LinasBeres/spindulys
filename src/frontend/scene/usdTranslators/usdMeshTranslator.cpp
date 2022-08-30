#include "usdMeshTranslator.h"

#include <iostream>

#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/imaging/hd/meshUtil.h>

#include <spindulys/math/affinespace.h>

#include "../../geometry/mesh.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

/* Reimplementation of some USD's Hd API functions to triangulate/quadriangulate meshes,
	 so they can be used outside of the Hd API. */
inline bool DoFanTriangulation(pxr::GfVec3i& triangulatedIndices,
		const pxr::VtArray<int>& meshVertexIndices,
		int meshVertexIndicesOffset,
		int meshVertexIndicesIdx,
		int meshIndicesCount,
		bool flipWindingOrder)
{
	// We check the topology.
	if (meshVertexIndicesOffset + meshVertexIndicesIdx + 2 >= meshIndicesCount)
	{
		triangulatedIndices = pxr::GfVec3i(0, 0, 0);

		return false;
	}

	if (flipWindingOrder)
	{
		triangulatedIndices = pxr::GfVec3i(meshVertexIndices[meshVertexIndicesOffset],
				meshVertexIndices[meshVertexIndicesOffset + meshVertexIndicesIdx + 2],
				meshVertexIndices[meshVertexIndicesOffset + meshVertexIndicesIdx + 1]);
	}
	else
	{
		triangulatedIndices = pxr::GfVec3i(meshVertexIndices[meshVertexIndicesOffset],
				meshVertexIndices[meshVertexIndicesOffset + meshVertexIndicesIdx + 1],
				meshVertexIndices[meshVertexIndicesOffset + meshVertexIndicesIdx + 2]);
	}

	return true;
}

inline pxr::VtVec3iArray TriangulateMeshIndices(pxr::VtArray<int>& meshVertexCounts,
		const pxr::VtArray<int>& meshVertexIndices,
		const pxr::VtArray<int>& meshHoleIndices,
		const pxr::TfToken& meshOrientation)
{
	pxr::VtVec3iArray meshTriangulatedIndices;

	int meshFacesCount = meshVertexCounts.size();
	int meshIndicesCount = meshVertexIndices.size();
	int meshHoleIndicesCount = meshHoleIndices.size();
	int meshTrianglesCount = 0;
	int meshHoleIdx = 0;
	bool flipWindingOrder = (meshOrientation != pxr::TfToken("rightHanded"));
	bool invalidTopology = false;

	for (int faceIdx = 0; faceIdx < meshFacesCount; ++faceIdx)
	{
		const int trianglesPerFace = meshVertexCounts[faceIdx] - 2;

		// We skip the broken faces
		if (trianglesPerFace < 1)
			invalidTopology = true;
		// We skip the hole faces
		else if (meshHoleIdx < meshHoleIndicesCount && meshHoleIndices[meshHoleIdx] == faceIdx)
			++meshHoleIdx;
		else
			meshTrianglesCount += trianglesPerFace;
	}

	if (invalidTopology)
	{
		std::cout << std::string("WARNING - Broken faces have been found.") << std::endl;
		invalidTopology = false;
	}

	meshTriangulatedIndices.resize(meshTrianglesCount);
	meshHoleIdx = 0;

	for (int faceIdx = 0, triangleFaceIdx = 0, faceVertexIdx = 0; faceIdx < meshFacesCount; ++faceIdx)
	{
		int faceVertexCount = meshVertexCounts[faceIdx];

		if (faceVertexCount < 3)
		{
			// We skip the invalid triangle faces.
		}
		else if (meshHoleIdx < meshHoleIndicesCount && meshHoleIndices[meshHoleIdx] == faceIdx)
		{
			// We skip the hole faces.
			++meshHoleIdx;
		}
		else
		{
			for (int faceVertexCountIdx = 0; faceVertexCountIdx < faceVertexCount - 2; ++faceVertexCountIdx)
			{
				meshTriangulatedIndices[triangleFaceIdx] = pxr::GfVec3i(0, 0, 0);

				if (!DoFanTriangulation(meshTriangulatedIndices[triangleFaceIdx],
							meshVertexIndices,
							faceVertexIdx,
							faceVertexCountIdx,
							meshIndicesCount,
							flipWindingOrder))
				{
					invalidTopology = true;
				}

				if (faceVertexCount > 3)
				{
					if (faceVertexCountIdx == 0)
					{
						if (flipWindingOrder)
						{
							pxr::GfVec3i &triangulatedIndices = meshTriangulatedIndices[triangleFaceIdx];

							triangulatedIndices.Set(triangulatedIndices[1],
									triangulatedIndices[2],
									triangulatedIndices[0]);
						}
					}
					else if (faceVertexCountIdx == faceVertexCount - 3)
					{
						if (flipWindingOrder)
						{
							pxr::GfVec3i &triangulatedIndices = meshTriangulatedIndices[triangleFaceIdx];

							triangulatedIndices.Set(triangulatedIndices[2],
									triangulatedIndices[0],
									triangulatedIndices[1]);
						}
					}
				}

				++triangleFaceIdx;
			}
		}

		faceVertexIdx += faceVertexCount;
	}

	if (invalidTopology)
	{
		std::cout << std::string("WARNING - An inconsistency between the mesh faceVertexCounts and faceVertexIndices "
				"has been found.") << std::endl;
	}

	return meshTriangulatedIndices;
}

void* UsdMeshTranslator::GetObjectFromPrim(const pxr::UsdPrim& prim)
{
	Mesh* mesh = new Mesh(Geometry::GeometryTypes::Mesh, prim.GetName());

	const AffineSpace3f affine(usdGeomXformCache.GetLocalToWorldTransform(prim));
	mesh->SetTransfrom(affine);

	pxr::UsdGeomMesh usdGeom(prim);

	pxr::VtArray<pxr::GfVec3f> pxrPoints;
	if (usdGeom.GetPointsAttr().Get(&pxrPoints))
	{
		std::vector<Vec3f> points;
		for (const auto& point : pxrPoints)
			points.emplace_back(Vec3f(point[0], point[1], point[2]));

		mesh->SetPoints(points);
	}

	pxr::VtArray<int> pxrIndices;
	if (usdGeom.GetFaceVertexIndicesAttr().Get(&pxrIndices))
	{
		std::vector<int> indices;
		for (const auto& index : pxrIndices)
			indices.emplace_back(index);

		mesh->SetIndices(indices);
	}

	// TODO: Handle these...
	pxr::VtArray<int> holeCount;
	if(usdGeom.GetHoleIndicesAttr().Get(&holeCount))
	{
	}

	pxr::TfToken orientation;
	if(usdGeom.GetOrientationAttr().Get(&orientation))
	{
	}

	pxr::VtArray<int> indicesCounts;
	if (usdGeom.GetFaceVertexCountsAttr().Get(&indicesCounts))
	{
		const size_t indexCount = pxrIndices.size() / indicesCounts.size();
		mesh->SetMeshType(indexCount == 3 ? Mesh::MeshType::TriangleMesh : Mesh::MeshType::QuadMesh);
	}

	pxr::VtArray<pxr::GfVec3f> pxrDisplayColor;
	if (usdGeom.GetDisplayColorAttr().Get(&pxrDisplayColor))
	{
		const Col3f displayColor(pxrDisplayColor[0][0], pxrDisplayColor[0][1], pxrDisplayColor[0][2]);
		mesh->SetDisplayColor(displayColor);
	}

	// Triangulation
	bool isTriangleMesh((static_cast<float>(pxrIndices.size()) /
				static_cast<float>(indicesCounts.size()) == 3.0f) ? true : false);
	bool isQuadMesh((static_cast<float>(pxrIndices.size()) /
				static_cast<float>(indicesCounts.size()) == 4.0f) ? true : false);
	bool needTriangulate((!isTriangleMesh && !isQuadMesh) ? true : false);

	if (needTriangulate)
	{
		const pxr::VtVec3iArray triangulated = TriangulateMeshIndices(indicesCounts, pxrIndices, holeCount, orientation);

		std::vector<int> indices;
		for (const auto& triangulatedIndices : triangulated)
		{
			indices.emplace_back(triangulatedIndices[0]);
			indices.emplace_back(triangulatedIndices[1]);
			indices.emplace_back(triangulatedIndices[2]);
		}
		mesh->SetIndices(indices);
		mesh->SetMeshType(Mesh::MeshType::TriangleMesh);
	}

	return (void*)mesh;
}


FRONTEND_NAMESPACE_CLOSE_SCOPE
