#ifndef SMOOTH_CONDUCTOR_H
#define SMOOTH_CONDUCTOR_H

#include "../bsdf.h"

#include "../../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class SmoothConductor : virtual public BSDF
{
	public:
		SmoothConductor(float eta, float k, const Col3f& specularReflectance, const std::string& id)
			: m_eta(eta)
			, m_k(k)
			, m_specularReflectance(specularReflectance)
		{
			m_id = id;
			m_flags = (uint32_t) BSDFFlags::DeltaReflection | (uint32_t) BSDFFlags::FrontSide;
			m_components.push_back(m_flags);
		}
		~SmoothConductor() = default;

		// Get Methods
		float GetETA() const { return m_eta; }
		float GetK()   const { return m_k;   }

		const Col3f& GetSpecularReflectance() const { return m_specularReflectance; }

	protected:
		// Componets of the material's index of refraction.
		float m_eta = zero;
		float m_k = one;

		// Optional factor that can be used to modulate the specular reflection component.
		// For realism this parameter should never be touched.
		Col3f m_specularReflectance = one;

};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // SMOOTH_CONDUCTOR_H
