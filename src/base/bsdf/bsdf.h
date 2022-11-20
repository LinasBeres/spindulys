#ifndef BSDF_H
#define BSDF_H

#include <string>

#include <spindulys/math/vec3.h>
#include <spindulys/sampler.h>
#include <spindulys/fwd.h>

#include <utils/helperStructs.h>

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

/**
 * \brief Specifies the transport mode when sampling or
 * evaluating a scattering function
 */
enum class TransportMode : uint32_t
{
	/// Radiance transport
	Radiance,

	/// Importance transport
	Importance,

	/// Specifies the number of supported transport modes
	TransportModes = 2
};

/**
 * \brief This list of flags is used to classify the different types of lobes
 * that are implemented in a BSDF instance.
 *
 * They are also useful for picking out individual components, e.g., by setting
 * combinations in \ref BSDFContext::type_mask.
 */
enum class BSDFFlags : uint32_t
{
	// =============================================================
	//                      BSDF lobe types
	// =============================================================

	/// No flags set (default value)
	Empty                = 0x00000,

	/// 'null' scattering event, i.e. particles do not undergo deflection
	Null                 = 0x00001,

	/// Ideally diffuse reflection
	DiffuseReflection    = 0x00002,

	/// Ideally diffuse transmission
	DiffuseTransmission  = 0x00004,

	/// Glossy reflection
	GlossyReflection     = 0x00008,

	/// Glossy transmission
	GlossyTransmission   = 0x00010,

	/// Reflection into a discrete set of directions
	DeltaReflection      = 0x00020,

	/// Transmission into a discrete set of directions
	DeltaTransmission    = 0x00040,

	/// Reflection into a 1D space of directions
	Delta1DReflection    = 0x00080,

	/// Transmission into a 1D space of directions
	Delta1DTransmission  = 0x00100,

	// =============================================================
	//!                   Other lobe attributes
	// =============================================================

	/// The lobe is not invariant to rotation around the normal
	Anisotropic          = 0x01000,

	/// The BSDF depends on the UV coordinates
	SpatiallyVarying     = 0x02000,

	/// Flags non-symmetry (e.g. transmission in dielectric materials)
	NonSymmetric         = 0x04000,

	/// Supports interactions on the front-facing side
	FrontSide            = 0x08000,

	/// Supports interactions on the back-facing side
	BackSide             = 0x10000,

	/// Does the implementation require access to texture-space differentials
	NeedsDifferentials   = 0x20000,

	// =============================================================
	//!                 Compound lobe attributes
	// =============================================================

	/// Any reflection component (scattering into discrete, 1D, or 2D set of directions)
	Reflection   = DiffuseReflection | DeltaReflection |
		Delta1DReflection | GlossyReflection,

	/// Any transmission component (scattering into discrete, 1D, or 2D set of directions)
	Transmission = DiffuseTransmission | DeltaTransmission |
		Delta1DTransmission | GlossyTransmission | Null,

	/// Diffuse scattering into a 2D set of directions
	Diffuse      = DiffuseReflection | DiffuseTransmission,

	/// Non-diffuse scattering into a 2D set of directions
	Glossy       = GlossyReflection | GlossyTransmission,

	/// Scattering into a 2D set of directions
	Smooth       = Diffuse | Glossy,

	/// Scattering into a discrete set of directions
	Delta        = Null | DeltaReflection | DeltaTransmission,

	/// Scattering into a 1D space of directions
	Delta1D      = Delta1DReflection | Delta1DTransmission,

	/// Any kind of scattering
	All          = Diffuse | Glossy | Delta | Delta1D
};

struct BSDFContext
{
	// =============================================================
	//! @{ \name Fields
	// =============================================================

	/// Transported mode (radiance or importance)
	TransportMode mode = TransportMode::Radiance;

	/*
	 * Bit mask for requested BSDF component types to be sampled/evaluated
	 * The default value (equal to \ref BSDFFlags::All) enables all components.
	 */
	uint32_t type_mask = (uint32_t) 0x1FFu;

	/// Integer value of requested BSDF component index to be sampled/evaluated.
	uint32_t component = (uint32_t) -1;

	//! @}
	// =============================================================

	BSDFContext() = default;

	BSDFContext(TransportMode mode, uint32_t type_mask = (uint32_t) 0x1FFu,
			uint32_t component = (uint32_t) -1)
		: mode(mode), type_mask(type_mask), component(component) { }

	/**
	 * \brief Reverse the direction of light transport in the record
	 *
	 * This updates the transport mode (radiance to importance and vice versa).
	 */
	void Reverse() {
		mode = (TransportMode) (1 - (int) mode);
	}

	/**
	 * Checks whether a given BSDF component type and BSDF component index are
	 * enabled in this context.
	 */
	bool IsEnabled(BSDFFlags type_, uint32_t component_ = 0) const {
		uint32_t type = (uint32_t) type_;
		return (type_mask == (uint32_t) -1 || (type_mask & type) == type)
			&& (component == (uint32_t) -1 || component == component_);
	}
};

// Data structure holding the result of BSDF sampling operations.
struct BSDFSample
{
	/// Normalized outgoing direction in local coordinates
	Vec3f wo;

	/// Probability density at the sample
	float pdf;

	/// Relative index of refraction in the sampled direction
	float eta;

	/// Stores the component type that was sampled by \ref BSDF::sample()
	uint32_t sampledType;

	/// Stores the component index that was sampled by \ref BSDF::sample()
	uint32_t sampledComponent;

	/**
	 * \brief Given a surface interaction and an incident/exitant direction
	 * pair (wi, wo), create a query record to evaluate the BSDF or its
	 * sampling density.
	 *
	 * By default, all components will be sampled regardless of what measure
	 * they live on.
	 *
	 * \param wo
	 *      An outgoing direction in local coordinates. This should
	 *      be a normalized direction vector that points \a away from
	 *      the scattering event.
	 */
	BSDFSample(const Vec3f& wo = zero)
		: wo(wo), pdf(0.f), eta(1.f), sampledType(0),
		sampledComponent(uint32_t(-1)) { }
};

struct SurfaceInteraction;

class BSDF
{
	public:
		virtual ~BSDF() = default;

		// Get Methods
		uint32_t           GetFlags()         const { return m_flags; }
		uint32_t           GetFlags(size_t i) const { assert(i < m_components.size()); return m_components[i]; }
		size_t             GetComponentSize() const { return m_components.size(); }
		const std::string& GetID()            const { return m_id; }

	protected:
		BSDF(const std::string& id) : m_flags(+BSDFFlags::Empty) , m_id(id) { }
		BSDF() = default;

	protected:
		/// Combined flags for all components of this BSDF.
		uint32_t m_flags;

		/// Flags for each component of this BSDF.
		std::vector<uint32_t> m_components;

		/// Identifier (if available)
		std::string m_id;
	private:
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // BSDF_H
