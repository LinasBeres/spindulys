#include "usdCameraTranslator.h"

#include <iostream>

#include <pxr/base/gf/camera.h>
#include "pxr/usd/usdGeom/camera.h"

#include <spindulys/math/affinespace.h>

#include "../../camera/camera.h"

BASE_NAMESPACE_OPEN_SCOPE


void* UsdCameraTranslator::GetObjectFromPrim(const pxr::UsdPrim& prim)
{
	BASE_TRACE();
	Camera* camera = new Camera(prim.GetName());

	pxr::UsdGeomCamera usdCamera(prim);
	const AffineSpace3f affine(usdCamera.ComputeLocalToWorldTransform(pxr::UsdTimeCode::Default()));
	camera->SetAffine(affine);

	pxr::TfToken projection;
	if (GetAttribute(prim, pxr::TfToken("projection"), &projection))
		camera->SetProjection(projection == pxr::TfToken("perspective") ? Camera::Projection::Perspective : Camera::Projection::Orthographic);

	float horizontalAperture;
	if (GetAttribute(prim, pxr::TfToken("horizontalAperture"), &horizontalAperture))
		camera->SetHorizontalAperature(horizontalAperture);

	float verticalAperture;
	if (GetAttribute(prim, pxr::TfToken("verticalAperature"), &verticalAperture))
		camera->SetVerticalAperature(verticalAperture);

	float horizontalApertureOffset;
	if (GetAttribute(prim, pxr::TfToken("horizontalApertureOffset"), &horizontalApertureOffset))
		camera->SetHorizontalAperatureOffset(horizontalApertureOffset);

	float verticalApertureOffset;
	if (GetAttribute(prim, pxr::TfToken("verticalApertureOffset"), &verticalApertureOffset))
		camera->SetVerticalAperatureOffset(verticalApertureOffset);

	float focalLength;
	if (GetAttribute(prim, pxr::TfToken("focalLength"), &focalLength))
		camera->SetFocalLength(focalLength);

	pxr::GfVec2f clippingRange;
	if (GetAttribute(prim, pxr::TfToken("clippingRange"), &clippingRange))
	{
		camera->SetClose(clippingRange[0]);
		camera->SetFar(clippingRange[1]);
	}

	// TODO: Clipping Planes

	float fStop;
	if (GetAttribute(prim, pxr::TfToken("fStop"), &fStop))
		camera->SetFStop(fStop);

	float focusDistance;
	if (GetAttribute(prim, pxr::TfToken("focusDistance"), &focusDistance))
		camera->SetFocusDistance(focusDistance);

	return (void*) camera;
}

BASE_NAMESPACE_CLOSE_SCOPE
