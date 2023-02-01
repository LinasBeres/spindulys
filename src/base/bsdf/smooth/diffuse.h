#ifndef SMOOTH_DIFFUSE_H
#define SMOOTH_DIFFUSE_H

#include <spindulys/math/col3.h>

#include "../bsdf.h"

#include "../../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class SmoothDiffuse : virtual public BSDF
{
	public:
		SmoothDiffuse(const Col3f& reflectance, const std::string& id)
			: m_reflectance(reflectance)
		{
			m_id = id;
			m_flags = BSDFFlags::DiffuseReflection | BSDFFlags::FrontSide;
			m_components.push_back(m_flags);
		}
		~SmoothDiffuse() = default;

		// Get Methods
		const Col3f& GetReflectance() const { return m_reflectance; }

	protected:
		Col3f m_reflectance = Col3f(0.5f);
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // SMOOTH_DIFFUSE_H
