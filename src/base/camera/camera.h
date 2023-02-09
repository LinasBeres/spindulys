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
			Right,
			None
		};

		Camera(const std::string& name);
		virtual ~Camera();

		void UpdateAxis();
		void KeyboardCallback(CAMERA_MOVEMENTS direction);
		void MouseCallback(const Vec2f& mouseOffset);

		virtual bool GetCameraRay(const Vec2f& sample, Vec3f& origin, Vec3f& direction) const;

		// Set Methods
		bool SetName(const std::string& name) { return name != std::exchange(m_name, name); }

		bool SetAffine(const AffineSpace3f& affine);
		bool SetResolution(const Vec2f& resolution);
		bool SetHorizontalAperature(float horizontalAperature);
		bool SetVerticalAperature(float verticalAperature);
		bool SetFocalLength(float focalLength);

		// TODO: Do something with these
		bool SetProjection(Projection projection)                          { return projection                != std::exchange(m_projection, projection);                               }
		bool SetHorizontalAperatureOffset(float horizontalAperatureOffset) { return horizontalAperatureOffset != std::exchange(m_horizontalAperatureOffset, horizontalAperatureOffset); }
		bool SetVerticalAperatureOffset(float verticalAperatureOffset)     { return verticalAperatureOffset   != std::exchange(m_verticalAperatureOffset, verticalAperatureOffset);     }
		bool SetFar(float far)                                             { return far                       != std::exchange(m_far, far);                                             }
		bool SetClose(float close)                                         { return close                     != std::exchange(m_close, close);                                         }
		bool SetFStop(float fStop)                                         { return fStop                     != std::exchange(m_fStop, fStop);                                         }
		bool SetFocusDistance(float focusDistance)                         { return focusDistance             != std::exchange(m_focusDistance, focusDistance);                         }

		// Get Methods
		const std::string&   GetName()                      const { return m_name;                      }
		const Vec2f&         GetResolution()                const { return m_resolution;                }
		const Vec3f&         GetPosition()                  const { return m_affine.p;                  }
		const LinearSpace3f& GetRotation()                  const { return m_affine.l;                  }
		Projection           GetProjection()                const { return m_projection;                }
		float                GetHorizontalAperature()       const { return m_horizontalAperature;       }
		float                GetVerticalAperature()         const { return m_verticalAperature;         }
		float                GetHorizontalAperatureOffset() const { return m_horizontalAperatureOffset; }
		float                GetFocalLength()               const { return m_focalLength;               }
		float                GetFar()                       const { return m_far;                       }
		float                GetClose()                     const { return m_close;                     }
		float                GetFStop()                     const { return m_fStop;                     }
		float                GetFocusDistance()             const { return m_focusDistance;             }

		float GetYaw()         const { return m_yaw;         }
		float GetPitch()       const { return m_pitch;       }
		float GetSpeed()       const { return m_speed;      }
		float GetSensitivity() const { return m_sensitivity; }

	protected:
		std::string m_name;

		Vec2f m_resolution = Vec2f(800.f, 600.f);

		// Frustum
		AffineSpace3f m_affine = AffineSpace3f(one, Vec3f(0.f, 5.f, 15.f));
		Projection    m_projection = Projection::Perspective;
		FOVDirection  m_fovDirecion = FOVDirection::FOVHorizontal;
		float         m_horizontalAperature = 32.955f;
		float         m_verticalAperature = 11.3284f;
		float         m_horizontalAperatureOffset = 0.f;
		float         m_verticalAperatureOffset = 0.f;
		float         m_focalLength = 35.f;

		float         m_far = 100.f;
		float         m_close = 0.1f;

		// TODO: Add clipping planes
		// std::vector<Vec4f>    _clippingPlanes;

		// Focus
		float         m_fStop = 0.f;
		float         m_focusDistance = 0.f;

		// Render info
		// Right handed rule.
		Vec3f m_zAxis = Vec3f(0.f, 0.f, -1.f);
		Vec3f m_xAxis = cross(m_zAxis, kUp);
		Vec3f m_yAxis = cross(m_zAxis, m_xAxis);

		float m_aperatureAspectRatio = m_horizontalAperature / m_verticalAperature;
		float m_deviceAspectRatio = m_resolution.x / m_resolution.y;

		Vec3f m_top   = m_yAxis * (m_horizontalAperature / (2.f * m_focalLength));
		Vec3f m_right = m_xAxis * (m_horizontalAperature / (2.f * m_focalLength)) * m_deviceAspectRatio;

		// Controls
		float m_yaw = 90.f;
		float m_pitch = 0.f;
		float m_speed = 0.1f;
		float m_sensitivity = 0.1f;
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // CAMERA_H
