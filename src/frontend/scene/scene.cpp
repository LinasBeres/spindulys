#include "scene.h"

#include <vector>

#include <tbb/blocked_range2d.h>
#include <tbb/blocked_range3d.h>
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>

#include "../geometry/trianglemesh.h"
#include "../geometry/quadmesh.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

bool Scene::LoadScene(const std::string& filepath)
{
	if (filepath.empty())
		return false;

	if (!((std::size_t(filepath.rfind(std::string(".usd")) != std::string::npos) ||
					std::size_t(filepath.rfind(std::string(".usda")) != std::string::npos) ||
					std::size_t(filepath.rfind(std::string(".usdc")) != std::string::npos) ||
					std::size_t(filepath.rfind(std::string(".usdz")) != std::string::npos)) ? true : false))
	{
		std::cerr << "ERROR - The following file is not an USD scene: " << filepath << std::endl;

		return false;
	}

	const pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(filepath);
	if (!stage)
		return false;

	LoadMeshGeometry(stage);

	return true;
}

void GetPrimFromType(const std::string& primType, const pxr::UsdStageRefPtr& stage, const pxr::SdfPath& primPath, std::vector<pxr::UsdPrim>& primVector)
{
	const pxr::UsdPrim basePrim = stage->GetPrimAtPath(primPath);
	for(const pxr::UsdPrim& prim: basePrim.GetChildren())
	{
		if (prim.GetTypeName() == primType)
			primVector.push_back(prim);

		if (prim.GetChildren())
			GetPrimFromType(primType, stage, prim.GetPath(), primVector);
	}
}

bool Scene::LoadMeshGeometry(const pxr::UsdStagePtr& stage)
{
	std::vector<pxr::UsdPrim> meshPrims;

	GetPrimFromType("Mesh", stage, pxr::SdfPath("/"), meshPrims);

	pxr::UsdGeomXformCache usdGeomXformCache;

	tbb::parallel_for_each(meshPrims.begin(), meshPrims.end(), [&](pxr::UsdPrim& prim)
			{
			pxr::UsdGeomMesh usdGeom(pxr::UsdGeomMesh::Get(stage, prim.GetPrimPath()));

			pxr::VtArray<pxr::GfVec3f> points;
			pxr::VtArray<int> indicesCounts;
			pxr::VtArray<int> indices;

			usdGeom.GetPointsAttr().Get(&points);
			usdGeom.GetFaceVertexIndicesAttr().Get(&indices);
			usdGeom.GetFaceVertexCountsAttr().Get(&indicesCounts);

			const std::string primName = prim.GetName();
			const AffineSpace3f affine(usdGeomXformCache.GetLocalToWorldTransform(prim));

			// TODO: Get the display color from the correct time value.
			pxr::VtArray<pxr::GfVec3f> pxrDisplayColor;
			usdGeom.GetDisplayColorAttr().Get(&pxrDisplayColor);
			const Col3f displayColor = (pxrDisplayColor.empty() ? Col3f(0.5f) :
					Col3f(pxrDisplayColor[0][0],
								pxrDisplayColor[0][1],
								pxrDisplayColor[0][2]));

			if (indices.size() / indicesCounts.size() == 3)
				CreateGeomerty(Geometry::GeometryTypes::TriangleMesh, primName, affine, displayColor, points, indices);
			else if (indices.size() / indicesCounts.size() == 4)
				CreateGeomerty(Geometry::GeometryTypes::QuadMesh, primName, affine, displayColor, points, indices);
			else
				std::cerr << "FIXME...\n";
				// TODO
			});

	CommitScene();

	return true;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE

