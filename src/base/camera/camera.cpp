#include "camera.h"

#include <spindulys/math/math.h>

BASE_NAMESPACE_OPEN_SCOPE

Camera::Camera(const std::string& name)
	: m_name(name)
{
}

Camera::~Camera()
{
}

bool Camera::GetCameraRay(const Vec2f& sample, Vec3f& origin, Vec3f& direction) const
{
	const float pointX((sample.x) / (GetResolution().x));
	const float pointY((sample.y) / (GetResolution().y));

	const Vec3f rayDirection(m_zAxis + (m_right * ((2.0f * pointX) - 1.0f)) + (m_top * ((2.0f * pointY) - 1.0f)));

	// This may change if the focus distance is not 0 hence why it is not const.
	Vec3f aperturePoint(GetPosition());

	// TODO: Do something if the focus distance is not 0.

	origin = aperturePoint;
	direction = normalize(rayDirection);

	return true;
}

// -----------------------------------------------------
// Mouse Callbacks
// -----------------------------------------------------
void Camera::KeyboardCallback(CAMERA_MOVEMENTS direction)
{
	BASE_TRACE();

	switch (direction)
	{
		case Forward:
			m_affine.p += m_zAxis * m_speed;
			break;
		case Backward:
			m_affine.p -= m_zAxis * m_speed;
			break;
		case Left:
			m_affine.p -= m_xAxis * m_speed;
			break;
		case Right:
			m_affine.p += m_xAxis * m_speed;
			break;
		default:
			break;
	}
}

void Camera::MouseCallback(const Vec2f& mouseOffset)
{
	BASE_TRACE();
	m_yaw += mouseOffset.x * m_sensitivity;
	m_pitch += mouseOffset.y * m_sensitivity;

	if (m_pitch > 89.0f)
	{
		m_pitch = 89.0f;
	}
	if (m_pitch < -89.0f)
	{
		m_pitch = -89.0f;
	}

	UpdateAxis();
}

// -----------------------------------------------------
// Update Methods
// -----------------------------------------------------
void Camera::UpdateAxis()
{
	BASE_TRACE();
	Vec3f front(
			cos(deg2rad(m_yaw)) * cos(deg2rad(m_pitch)),
			sin(deg2rad(m_pitch)),
			sin(deg2rad(m_yaw)) * cos(deg2rad(m_pitch))
			);

	front *= -1.0f;

	m_zAxis = normalize(front);

	m_xAxis= cross(m_zAxis, kUp);
	m_yAxis = cross(m_zAxis, m_xAxis);

	// Apply camera rotation if any.
	m_zAxis = xfmVector(m_affine, m_zAxis);
	m_xAxis = xfmVector(m_affine, m_xAxis);
	m_yAxis = xfmVector(m_affine, m_yAxis);

	m_top   = m_yAxis * (m_horizontalAperature / (2.f * m_focalLength));
	m_right = m_xAxis * (m_horizontalAperature / (2.f * m_focalLength)) * m_deviceAspectRatio;
}

bool Camera::SetAffine(const AffineSpace3f& affine)
{
	if (affine == std::exchange(m_affine, affine))
		return false;

	UpdateAxis();
	return true;
}

bool Camera::SetHorizontalAperature(float horizontalAperature)
{
	BASE_TRACE();
	if (horizontalAperature == std::exchange(m_horizontalAperature, horizontalAperature))
		return false;

	if (m_fovDirecion == FOVDirection::FOVHorizontal)
	{
		m_top   = m_yAxis * (m_horizontalAperature / (2.f * m_focalLength));
		m_right = m_xAxis * (m_horizontalAperature / (2.f * m_focalLength)) * m_deviceAspectRatio;
	}

	m_aperatureAspectRatio = m_horizontalAperature >= m_verticalAperature ?
		m_horizontalAperature / m_verticalAperature : m_verticalAperature / m_horizontalAperature;

	return true;
}

bool Camera::SetVerticalAperature(float verticalAperature)
{
	BASE_TRACE();
	if (verticalAperature == std::exchange(m_verticalAperature, verticalAperature))
		return false;

	if (m_fovDirecion == FOVDirection::FOVVertical)
	{
		m_top   = m_yAxis * (m_verticalAperature/ (2.f * m_focalLength));
		m_right = m_xAxis * (m_verticalAperature/ (2.f * m_focalLength)) * m_deviceAspectRatio;
	}

	m_aperatureAspectRatio = m_horizontalAperature >= m_verticalAperature ?
		m_horizontalAperature / m_verticalAperature : m_verticalAperature / m_horizontalAperature;

	return true;
}

bool Camera::SetFocalLength(float focalLength)
{
	BASE_TRACE();
	if (focalLength == std::exchange(m_focalLength, focalLength))
		return false;

	if (m_fovDirecion == FOVDirection::FOVHorizontal)
	{
		m_top   = m_yAxis * (m_horizontalAperature / (2.f * m_focalLength));
		m_right = m_xAxis * (m_horizontalAperature / (2.f * m_focalLength)) * m_deviceAspectRatio;
	}
	else if (m_fovDirecion == FOVDirection::FOVVertical)
	{
		m_top   = m_yAxis * (m_verticalAperature/ (2.f * m_focalLength));
		m_right = m_xAxis * (m_verticalAperature/ (2.f * m_focalLength)) * m_deviceAspectRatio;
	}

	return true;
}

bool Camera::SetResolution(const Vec2f& resolution)
{
	BASE_TRACE();
	if (m_resolution == std::exchange(m_resolution, resolution))
		return false;

	m_deviceAspectRatio = m_resolution.x >= m_resolution.y ?
		m_resolution.x / m_resolution.y : m_resolution.y / m_resolution.x;

	UpdateAxis();
	return true;
}

BASE_NAMESPACE_CLOSE_SCOPE
