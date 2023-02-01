#ifndef CAMERA_H
#define CAMERA_H

#include <utility>

#include <spindulys/math/vec2.h>
#include <spindulys/math/vec3.h>
#include <spindulys/math/affinespace.h>

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class Camera
{
	public:
		static constexpr double kAperatureUnit = 0.1f;
		static constexpr float kFocalLengthUnit = 0.1f;

		const Vec3f kUp = Vec3f(0.f, 1.f, 0.f);

		enum Projection {
			Perspective = 0,
			Orthographic,
		};

		/// Direction used for Field of View or orthographic size
		enum FOVDirection {
			FOVHorizontal = 0,
			FOVVertical
		};

		enum CAMERA_MOVEMENTS
		{
			Forward,
			Backward,
			Left,
			Right
		};

		Camera(const std::string& name);
		virtual ~Camera();

		void UpdateAxis();
		void KeyboardCallback(CAMERA_MOVEMENTS direction, float deltaTime);
		void MouseCallback(const Vec2f& mouseOffset);

		virtual bool GetCameraRay(const Vec2f& sample, Vec3f& origin, Vec3f& direction) const;

		// Set Methods
		bool SetName(const std::string& name) { return name != std::exchange(_name, name); }

		bool SetAffine(const AffineSpace3f& affine);
		bool SetResolution(const Vec2f& resolution);
		bool SetHorizontalAperature(float horizontalAperature);
		bool SetVerticalAperature(float verticalAperature);
		bool SetFocalLength(float focalLength);

		// TODO: Do something with these
		bool SetProjection(Projection projection)                          { return projection                != std::exchange(_projection, projection);                               }
		bool SetHorizontalAperatureOffset(float horizontalAperatureOffset) { return horizontalAperatureOffset != std::exchange(_horizontalAperatureOffset, horizontalAperatureOffset); }
		bool SetVerticalAperatureOffset(float verticalAperatureOffset)     { return verticalAperatureOffset   != std::exchange(_verticalAperatureOffset, verticalAperatureOffset);     }
		bool SetFar(float far)                                             { return far                       != std::exchange(_far, far);                                             }
		bool SetClose(float close)                                         { return close                     != std::exchange(_close, close);                                         }
		bool SetFStop(float fStop)                                         { return fStop                     != std::exchange(_fStop, fStop);                                         }
		bool SetFocusDistance(float focusDistance)                         { return focusDistance             != std::exchange(_focusDistance, focusDistance);                         }

		// Get Methods
		const std::string&   GetName()                      const { return _name;                      }
		const Vec2f&         GetResolution()                const { return _resolution;                }
		const Vec3f&         GetPosition()                  const { return _affine.p;                  }
		const LinearSpace3f& GetRotation()                  const { return _affine.l;                  }
		Projection           GetProjection()                const { return _projection;                }
		float                GetHorizontalAperature()       const { return _horizontalAperature;       }
		float                GetVerticalAperature()         const { return _verticalAperature;         }
		float                GetHorizontalAperatureOffset() const { return _horizontalAperatureOffset; }
		float                GetFocalLength()               const { return _focalLength;               }
		float                GetFar()                       const { return _far;                       }
		float                GetClose()                     const { return _close;                     }
		float                GetFStop()                     const { return _fStop;                     }
		float                GetFocusDistance()             const { return _focusDistance;             }

		float GetYaw()         const { return _yaw;         }
		float GetPitch()       const { return _pitch;       }
		float GetSpeed()       const { return _speed;       }
		float GetSensitivity() const { return _sensitivity; }

	protected:
		std::string _name;

		Vec2f _resolution = Vec2f(800.f, 600.f);

		// Frustum
		AffineSpace3f _affine = AffineSpace3f(one, Vec3f(0.f, 5.f, 15.f));
		Projection    _projection = Projection::Perspective;
		FOVDirection  _fovDirecion = FOVDirection::FOVHorizontal;
		float         _horizontalAperature = 32.955f;
		float         _verticalAperature = 11.3284f;
		float         _horizontalAperatureOffset = 0.f;
		float         _verticalAperatureOffset = 0.f;
		float         _focalLength = 35.f;

		float         _far = 100.f;
		float         _close = 0.1f;

		// TODO: Add clipping planes
		// std::vector<Vec4f>    _clippingPlanes;

		// Focus
		float         _fStop = 0.f;
		float         _focusDistance = 0.f;

		// Render info
		// Right handed rule.
		Vec3f _zAxis = Vec3f(0.f, 0.f, -1.f);
		Vec3f _xAxis = cross(_zAxis, kUp);
		Vec3f _yAxis = cross(_zAxis, _xAxis);

		float _aperatureAspectRatio = _horizontalAperature / _verticalAperature;
		float _deviceAspectRatio = _resolution.x / _resolution.y;

		Vec3f _top   = _yAxis * (_horizontalAperature / (2.f * _focalLength));
		Vec3f _right = _xAxis * (_horizontalAperature / (2.f * _focalLength)) * _deviceAspectRatio;

		// Controls
		float _yaw = 90.f;
		float _pitch = 0.f;
		float _speed = 5.f;
		float _sensitivity = 0.1f;
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // CAMERA_H
