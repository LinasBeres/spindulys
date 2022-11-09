#include "diffuse.h"

BASE_NAMESPACE_OPEN_SCOPE

SmoothDiffuse::SmoothDiffuse(const Col3f& reflectance, const std::string& id)
	: BSDF(id)
	, m_reflectance(reflectance)
{
	m_flags = (uint32_t) BSDFFlags::DiffuseReflection | (uint32_t) BSDFFlags::FrontSide;
	m_components.push_back(m_flags);
}

BASE_NAMESPACE_CLOSE_SCOPE
