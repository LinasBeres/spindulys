#include "usdBasisCurveTranslator.h"

#include <iostream>

#include "pxr/usd/usdGeom/basisCurves.h"

#include <spindulys/math/affinespace.h>

#include "../../geometry/curve.h"

FRONTEND_NAMESPACE_OPEN_SCOPE

void* UsdBasisCurveTranslator::GetObjectFromPrim(const pxr::UsdPrim& prim)
{
	FRONTEND_TRACE();
	Curve* curve = new Curve(Geometry::GeometryTypes::Curve, prim.GetName());

	pxr::UsdGeomBasisCurves usdBasisCurve(prim);
	const AffineSpace3f affine(usdBasisCurve.ComputeLocalToWorldTransform(pxr::UsdTimeCode::Default()));
	curve->SetTransfrom(affine);

	pxr::VtArray<pxr::GfVec3f> pxrPoints;
	if (usdBasisCurve.GetPointsAttr().Get(&pxrPoints))
	{
		std::vector<Vec3f> points;
		for (const auto& point : pxrPoints)
			points.emplace_back(Vec3f(point[0], point[1], point[2]));

		curve->SetPoints(points);
	}

	bool hasNormals = false;
	pxr::VtArray<pxr::GfVec3f> pxrNormals;
	if (usdBasisCurve.GetNormalsAttr().Get(&pxrNormals))
	{
		std::vector<Vec3f> normals;
		for (const auto& normal : pxrNormals)
			normals.emplace_back(Vec3f(normal[0], normal[1], normal[2]));

		if (normals.size() < curve->GetPoints().size())
		{
			spdlog::warn("We currently do not support varying curve normals. Skipping this curve.");
			delete curve;
			return nullptr;
		}


		curve->SetNormals(normals);
		hasNormals = true;
	}

	bool hasWidth = false;
	pxr::VtArray<float> pxrWidths;
	if (usdBasisCurve.GetWidthsAttr().Get(&pxrWidths))
	{
		std::vector<float> widths;
		if (pxrWidths.size() == 1)
			widths.assign(curve->GetPoints().size(), pxrWidths[0]);
		else
			for (const auto& width : pxrWidths)
				widths.emplace_back(width);

		if (widths.size() < curve->GetPoints().size())
		{
			spdlog::warn("We currently do not support varying curve widths. Skipping this curve.");
			delete curve;
			return nullptr;
		}

		curve->SetWidths(widths);
		hasWidth = true;
	}

	pxr::TfToken pxrType("cubic");
	usdBasisCurve.GetTypeAttr().Get(&pxrType);

	pxr::TfToken pxrBasis("bezier");
	usdBasisCurve.GetBasisAttr().Get(&pxrBasis);

	if (pxrType == pxr::TfToken("linear"))
	{
		curve->SetCurveType(Curve::CurveTypes::FlatLinear);
	}
	else if (hasWidth)
	{
		if (hasNormals)
		{
			if (pxrBasis == pxr::TfToken("bezier"))
				curve->SetCurveType(Curve::CurveTypes::NormalOrientatedBezier);
			else if (pxrBasis == pxr::TfToken("bspline"))
				curve->SetCurveType(Curve::CurveTypes::NormalOrientatedBezier);
			else
				spdlog::warn("Unkown basis type: {}", pxrBasis.GetString());
		}
		else
		{
			if (pxrBasis == pxr::TfToken("bezier"))
				curve->SetCurveType(Curve::CurveTypes::RoundBezier);
			else if (pxrBasis == pxr::TfToken("bspline"))
				curve->SetCurveType(Curve::CurveTypes::RoundBSpline);
			else
				std::cerr << "Unkown basis type: " << pxrBasis << "\n";
		}
	}
	else
	{
		if (pxrBasis == pxr::TfToken("bezier"))
			curve->SetCurveType(Curve::CurveTypes::FlatBezier);
		else if (pxrBasis == pxr::TfToken("bspline"))
			curve->SetCurveType(Curve::CurveTypes::FlatBSpline);
		else
			std::cerr << "Unkown basis type: " << pxrBasis << "\n";
	}

	pxr::VtArray<pxr::GfVec3f> pxrDisplayColor;
	if (usdBasisCurve.GetDisplayColorAttr().Get(&pxrDisplayColor))
	{
		const Col3f displayColor(pxrDisplayColor[0][0], pxrDisplayColor[0][1], pxrDisplayColor[0][2]);
		curve->SetDisplayColor(displayColor);
	}

	return curve;
}

FRONTEND_NAMESPACE_CLOSE_SCOPE
