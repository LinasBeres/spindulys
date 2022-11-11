#ifndef DIFFUSE_H
#define DIFFUSE_H

#include "bsdf.h"

#include "../spindulysBase.h"

BASE_NAMESPACE_OPEN_SCOPE

class SmoothDiffuse : virtual public BSDF
{
	public:
		SmoothDiffuse(const Col3f& reflectance, const std::string& id = "defaultSmoothDiffuse");
		~SmoothDiffuse() = default;

		// Get Methods
		const Col3f& GetReflectance() const { return m_reflectance; }

	protected:
		Col3f m_reflectance = Col3f(0.5f);
};

BASE_NAMESPACE_CLOSE_SCOPE

#endif // CPU_DIFFUSE_H
