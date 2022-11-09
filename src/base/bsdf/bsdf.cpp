#include "bsdf.h"

BASE_NAMESPACE_OPEN_SCOPE

BSDF::BSDF(const std::string& id)
	: m_flags((uint32_t)BSDFFlags::Empty)
	, m_id(id)
{ }

BASE_NAMESPACE_CLOSE_SCOPE
