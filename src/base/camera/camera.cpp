#include "camera.h"

#include <spindulys/math/math.h>

BASE_NAMESPACE_OPEN_SCOPE

Camera::Camera(const std::string& name)
	: _name(name)
{
}

Camera::~Camera()
{
}

bool Camera::GetCameraRay(const Vec2f& sample, Vec3f& origin, Vec3f& direction) const
{
	const float pointX((sample.x - 0.5f) / (GetResolution().x - 1.f));
	const float pointY((sample.y - 0.5f) / (GetResolution().y - 1.f));

	const Vec3f rayDirection(_zAxis + (_right * ((2.0f * pointX) - 1.0f)) + (_top * ((2.0f * pointY) - 1.0f)));

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
void Camera::KeyboardCallback(CAMERA_MOVEMENTS direction,
		float deltaTime)
{
	BASE_TRACE();
	float velocity(_speed * deltaTime);

	if (direction == Forward)
	{
		_affine.p += _zAxis * velocity;
	}
	if (direction == Backward)
	{
		_affine.p -= _zAxis * velocity;
	}
	if (direction == Left)
	{
		_affine.p -= _xAxis * velocity;
	}
	if (direction == Right)
	{
		_affine.p += _xAxis * velocity;
	}
}

void Camera::MouseCallback(const Vec2f& mouseOffset)
{
	BASE_TRACE();
	_yaw += mouseOffset.x * _sensitivity;
	_pitch += mouseOffset.y * _sensitivity;

	if (_pitch > 89.0f)
	{
		_pitch = 89.0f;
	}
	if (_pitch < -89.0f)
	{
		_pitch = -89.0f;
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
			cos(deg2rad(_yaw)) * cos(deg2rad(_pitch)),
			sin(deg2rad(_pitch)),
			sin(deg2rad(_yaw)) * cos(deg2rad(_pitch))
			);

	front *= -1.0f;

	_zAxis = normalize(front);

	_xAxis= cross(_zAxis, kUp);
	_yAxis = cross(_zAxis, _xAxis);

	// Apply camera rotation if any.
	_zAxis = xfmVector(_affine, _zAxis);
	_xAxis = xfmVector(_affine, _xAxis);
	_yAxis = xfmVector(_affine, _yAxis);

	_top   = _yAxis * (_horizontalAperature / (2.f * _focalLength));
	_right = _xAxis * (_horizontalAperature / (2.f * _focalLength)) * _deviceAspectRatio;
}

bool Camera::SetAffine(const AffineSpace3f& affine)
{
	if (affine == std::exchange(_affine, affine))
		return false;

	UpdateAxis();
	return true;
}

bool Camera::SetHorizontalAperature(float horizontalAperature)
{
	BASE_TRACE();
	if (horizontalAperature == std::exchange(_horizontalAperature, horizontalAperature))
		return false;

	if (_fovDirecion == FOVDirection::FOVHorizontal)
	{
		_top   = _yAxis * (_horizontalAperature / (2.f * _focalLength));
		_right = _xAxis * (_horizontalAperature / (2.f * _focalLength)) * _deviceAspectRatio;
	}

	_aperatureAspectRatio = _horizontalAperature >= _verticalAperature ?
		_horizontalAperature / _verticalAperature : _verticalAperature / _horizontalAperature;

	return true;
}

bool Camera::SetVerticalAperature(float verticalAperature)
{
	BASE_TRACE();
	if (verticalAperature == std::exchange(_verticalAperature, verticalAperature))
		return false;

	if (_fovDirecion == FOVDirection::FOVVertical)
	{
		_top   = _yAxis * (_verticalAperature/ (2.f * _focalLength));
		_right = _xAxis * (_verticalAperature/ (2.f * _focalLength)) * _deviceAspectRatio;
	}

	_aperatureAspectRatio = _horizontalAperature >= _verticalAperature ?
		_horizontalAperature / _verticalAperature : _verticalAperature / _horizontalAperature;

	return true;
}

bool Camera::SetFocalLength(float focalLength)
{
	BASE_TRACE();
	if (focalLength == std::exchange(_focalLength, focalLength))
		return false;

	if (_fovDirecion == FOVDirection::FOVHorizontal)
	{
		_top   = _yAxis * (_horizontalAperature / (2.f * _focalLength));
		_right = _xAxis * (_horizontalAperature / (2.f * _focalLength)) * _deviceAspectRatio;
	}
	else if (_fovDirecion == FOVDirection::FOVVertical)
	{
		_top   = _yAxis * (_verticalAperature/ (2.f * _focalLength));
		_right = _xAxis * (_verticalAperature/ (2.f * _focalLength)) * _deviceAspectRatio;
	}

	return true;
}

bool Camera::SetResolution(const Vec2f& resolution)
{
	BASE_TRACE();
	if (_resolution == std::exchange(_resolution, resolution))
		return false;

	_deviceAspectRatio = _resolution.x >= _resolution.y ?
		_resolution.x / _resolution.y : _resolution.y / _resolution.x;

	return true;
}

BASE_NAMESPACE_CLOSE_SCOPE
