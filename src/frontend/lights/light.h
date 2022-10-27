#ifndef LIGHT_H
#define LIGHT_H

#include "../spindulysFrontend.h"


FRONTEND_NAMESPACE_OPEN_SCOPE

/**
 * \brief This list of flags is used to classify the different types of emitters.
 */
enum class LightFlags : uint32_t {
	// =============================================================
	//                      Emitter types
	// =============================================================

	/// No flags set (default value)
	Empty                = 0x00000,

	/// The emitter lies at a single point in space
	DeltaPosition        = 0x00001,

	/// The emitter emits light in a single direction
	DeltaDirection       = 0x00002,

	/// The emitter is placed at infinity (e.g. environment maps)
	Infinite             = 0x00004,

	/// The emitter is attached to a surface (e.g. area emitters)
	Surface              = 0x00008,

	// =============================================================
	//!                   Other lobe attributes
	// =============================================================

	/// The emission depends on the UV coordinates
	SpatiallyVarying     = 0x00010,

	// =============================================================
	//!                 Compound lobe attributes
	// =============================================================

	/// Delta function in either position or direction
	Delta = DeltaPosition | DeltaDirection,
};

class Light
{
	public:
		Light() = default;
		virtual ~Light() = default;

		bool IsEnvironment() const
		{ return (m_flags & (uint32_t) LightFlags::Infinite) != 0 && (m_flags & (uint32_t) LightFlags::Infinite) == 0; }

		uint32_t GetFlags() const { return m_flags; }

	protected:
		uint32_t m_flags;

	private:
};

FRONTEND_NAMESPACE_CLOSE_SCOPE

#endif // LIGHT_H
