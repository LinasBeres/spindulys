#ifndef CAMERA_H
#define CAMERA_H

#include <utility>

#include <spindulys/math/vec2.h>
#include <spindulys/math/vec3.h>
#include <spindulys/sampler.h>

#include "../spindulysFrontend.h"

#include "../utils/helperStructs.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

class Camera
{
	public:
		enum Projection {
			Perspective = 0,
			Orthographic,
		};

		enum CAMERA_MOVEMENTS
		{
			Forward,
			Backward,
			Left,
			Right
		};

		Camera();
		virtual ~Camera();

		void Init();
		void Update();
		void SetupFOV();
		void KeyboardCallback(CAMERA_MOVEMENTS direction, float deltaTime);
		void MouseCallback(const Vec2f& mouseOffset);


		virtual bool GetCameraRay(const PixelSample& pixelSample, Vec3f& origin, Vec3f& direction) const = 0;

		// Set Methods
		// bool SetProjection(Projection projection);
		// bool SetHorizontalAperature(float horizontalAperature);
		// bool SetVerticalAperature(float verticalAperature);
		// bool SetHorizontalAperatureOffset(float horizontalApertatureOffset);
		// bool SetVerticalAperatureOffset(float verticalAperatureOffset);
		// bool SetFocalLength(float focalLength);
		// bool SetFar(float far);
		// bool SetClose(float close);
		// bool SetFStop(float fStop);
		// bool SetFocusDistance(float focusDistance);

		bool SetResolution(const Vec2f& resolution)    { return _resolution == std::exchange(_resolution, resolution); }
		bool SetJitter(bool jitter)                    { return _jitter == std::exchange(_jitter, jitter); }
		bool SetFocalDistance(float focalDistance)     { return _focalDistance == std::exchange(_focalDistance, focalDistance); }
		bool SetAperatureRadius(float aperatureRadius) { return _apertureRadius == std::exchange(_apertureRadius, aperatureRadius); }

		// Get Methods
		bool GetJitter() const { return _jitter; }
		float GetYaw() const { return _yaw; }
		float GetPitch() const { return _pitch; }
		float GetSpeed() const { return _speed; }
		float GetSensitivity() const { return _sensitivity; }
		float GetAperatureRadius() const { return _apertureRadius; }
		float GetFocalDistance() const { return _focalDistance; }
		const Vec2f& GetResolution() const { return _resolution; }
		const Vec2f& GetFov() const { return _fov; }
		const Vec3f& GetPosition() const { return _position; }
		const Vec3f& GetFront() const { return _front; }
		const Vec3f& GetUp() const { return _up; }
		const Vec3f& GetRight() const { return _right; }

	protected:
		// GfMatrix4d _transform;
		// Projection                    _projection;
		// float                         _horizontalAperature;
		// float                         _verticalAperature;
		// float                         _horizontalApertureOffset;
		// float                         _verticalApertureOffset;
		// float                         _focalLength;
		// float                         _far;
		// float                         _close;
		// std::vector<Vec4f>    _clippingPlanes;

		// focus
		// float                   _fStop;
		// float                   _focusDistance;


		bool _jitter = true;
		float _yaw;
		float _pitch;
		float _speed;
		float _sensitivity;
		float _apertureRadius;
		float _focalDistance;
		Vec2f _resolution;
		Vec2f _fov;
		Vec3f _position;
		Vec3f _front;
		Vec3f _up;
		Vec3f _right;
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // CAMERA_H
