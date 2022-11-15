#ifndef SMOOTH_PLASTIC_H
#define SMOOTH_PLASTIC_H

#include <spindulys/fresnel.h>

#include "../bsdf.h"

#include "../../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class SmoothPlastic : virtual public BSDF
{
	public:
		SmoothPlastic(float interiorIOR, float exteriorIOR,
				const Col3f& diffuseReflection, const Col3f& specularReflectance,
				bool nonLinear, const std::string& id)
			: m_diffuseReflectance(diffuseReflection)
			, m_specularReflectance(specularReflectance)
			, m_eta(interiorIOR / exteriorIOR)
			, m_nonLinear(nonLinear)
		{
			m_id = id;

			m_invETA2 = 1.f / (m_eta * m_eta);

			// Numerically approximate the diffuse Fresnel reflectance
			m_fdrInt = fresnel_diffuse_reflectance(1.f / m_eta);
			m_fdrExt = fresnel_diffuse_reflectance(m_eta);

			const float diffuseMean = (m_diffuseReflectance.r + m_diffuseReflectance.g + m_diffuseReflectance.b) / 3.f;
			const float specularMean = (m_specularReflectance.r + m_specularReflectance.g + m_specularReflectance.b) / 3.f;

			m_specularSamplingWeight = specularMean / (diffuseMean + specularMean);

			m_components.push_back((uint32_t) BSDFFlags::DeltaReflection   | (uint32_t) BSDFFlags::FrontSide);
			m_components.push_back((uint32_t) BSDFFlags::DiffuseReflection | (uint32_t) BSDFFlags::FrontSide);
			m_flags = m_components[0] | m_components[1];
		}
		~SmoothPlastic() = default;

		// Get Methods
		const Col3f& GetDiffuseReflectance()  const { return m_diffuseReflectance;  }
		const Col3f& GetSpecularReflectance() const { return m_specularReflectance; }

		float GetETA()                    const { return m_eta;                    }
		float GetInvETA2()                const { return m_invETA2;                }
		float GetFdrInt()                 const { return m_fdrInt;                 }
		float GetFrdExt()                 const { return m_fdrExt;                 }
		float GetSpecularSamplingWeight() const { return m_specularSamplingWeight; }

		bool IsNonLinear() const { return m_nonLinear; }

	protected:
		Col3f m_diffuseReflectance = Col3f(0.5f);
		Col3f m_specularReflectance = one;

		float m_eta = one;
		float m_invETA2;
		float m_fdrInt;
		float m_fdrExt;
		float m_specularSamplingWeight;

		bool m_nonLinear = false;
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // SMOOTH_PLASTIC_H
