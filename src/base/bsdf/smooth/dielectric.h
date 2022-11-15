#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include "../bsdf.h"

#include "../../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class SmoothDielectric : virtual public BSDF
{
	public:
		SmoothDielectric(float interiorIOR, float exteriorIOR,
				const Col3f& specularReflectance, const Col3f& specularTransmittance,
				const std::string& id)
			: m_eta(interiorIOR / exteriorIOR)
			, m_specularReflectance(specularReflectance)
			, m_specularTransmittance(specularTransmittance)
		{
			m_id = id;

			m_components.push_back(
					(uint32_t) BSDFFlags::DeltaReflection |
					(uint32_t) BSDFFlags::FrontSide |
					(uint32_t) BSDFFlags::BackSide
					);
			m_components.push_back(
					(uint32_t) BSDFFlags::DeltaTransmission |
					(uint32_t) BSDFFlags::FrontSide |
					(uint32_t) BSDFFlags::BackSide |
					(uint32_t) BSDFFlags::NonSymmetric);

			m_flags = m_components[0] | m_components[1];
		}
		~SmoothDielectric() = default;

		// Get Methods
		float GetETA() const { return m_eta; }

		const Col3f& GetSpecularReflection()    const { return m_specularReflectance;   }
		const Col3f& GetSpecularTransmittance() const { return m_specularTransmittance; }

	protected:
		// Relative index of refraction from the exterior to the interior.
		float m_eta = zero;

		// Optional factor that can be used to modulate the specular reflection/transmittance component.
		// For realism this parameter should never be touched.
		Col3f m_specularReflectance   = one;
		Col3f m_specularTransmittance = one;
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // DIELECTRIC_H
