#include "cpuCamera.h"

BACKEND_CPU_NAMESPACE_OPEN_SCOPE


bool CPUCamera::GetCameraRay(const PixelSample& pixelSample, Vec3f& origin, Vec3f& direction) const
{
	const Vec3f axisX(normalize(cross(GetFront(), GetUp())));
	const Vec3f axisY(normalize(cross(axisX, GetFront())));
	const Vec3f forward(GetPosition() + GetFront());
	const Vec3f vectorX(axisX * tan(GetFov().x * 0.5f * (float(M_PI) / 180.f)));
	const Vec3f vectorY(axisY * tan(GetFov().y * -0.5f * (float(M_PI) / 180.f)));

	const float pointX((((GetJitter() ? pixelSample.sampler.Uniform1D() : 0.0f) - 0.5f) + pixelSample.pixelX)
			/ (GetResolution().x - 1.0f));
	const float pointY((((GetJitter() ? pixelSample.sampler.Uniform1D() : 0.0f) - 0.5f) + pixelSample.pixelY)
			/ (GetResolution().y - 1.0f));

	// std::cerr << "FOX:" << GetFov() << "\n";

	const Vec3f pointOnPlane(GetPosition()
			+ ((forward + (vectorX * ((2.0f * pointX) - 1.0f))
				+ (vectorY * ((2.0f * pointY) - 1.0f))
				- GetPosition()) * GetFocalDistance()));

	Vec3f aperturePoint(GetPosition());
	if (GetAperatureRadius() > 0.0f)
	{
		const float randomAngle(2.0f * M_PI * pixelSample.sampler.Uniform1D());
		const float randomRadius(GetAperatureRadius() * sqrt(pixelSample.sampler.Uniform1D()));
		const float apertureX(cos(randomAngle) * randomRadius);
		const float apertureY(sin(randomAngle) * randomRadius);

		aperturePoint = GetPosition() + (axisX * apertureX) + (axisY * apertureY);
	}

	origin = aperturePoint;
	direction = normalize(pointOnPlane - aperturePoint);

	return true;;
}

BACKEND_CPU_NAMESPACE_CLOSE_SCOPE
