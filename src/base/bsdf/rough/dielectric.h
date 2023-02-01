#ifndef DIELECTRIC_H
#define DIELECTRIC_H

#include <spindulys/math/col3.h>

#include "../bsdf.h"

#include "../../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class SmoothDielectric : virtual public BSDF
{
	public:
		SmoothDielectric(const Col3f& reflectance, const std::string& id = "defaultSmoothDiffuse");
		~SmoothDielectric() = default;

		// Get Methods
		const Col3f& GetReflectance() const { return m_reflectance; }

	protected:
		Col3f m_reflectance = Col3f(0.5f);
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // DIELECTRIC_H
