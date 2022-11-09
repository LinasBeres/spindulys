#ifndef SPINDULYS_BASE_H
#define SPINDULYS_BASE_H

#include <spindulys.h>


#if SPINDULYS_USE_NAMESPACES

#define BASE_NAMESPACE_OPEN_SCOPE SPINDULYS_NAMESPACE_OPEN_SCOPE SPINDULYS_BASE_NAMESPACE_OPEN_SCOPE
#define BASE_NAMESPACE_CLOSE_SCOPE SPINDULYS_NAMESPACE_CLOSE_SCOPE SPINDULYS_BASE_NAMESPACE_CLOSE_SCOPE

#else

#define BASE_NAMESPACE_OPEN_SCOPE 
#define BASE_NAMESPACE_CLOSE_SCOPE 

#endif // SPINDULYS_USE_NAMESPACE

// Set to 0 to disable BASE tracing.
#define BASE_TRACING 0
#if BASE_TRACING
#define BASE_TRACE() MTR_SCOPE("BASE", __func__);
#define BASE_SCOPE(m) MTR_SCOPE("BASE", m);
#define BASE_BEGIN(m) MTR_BEGIN("BASE", m);
#define BASE_END(m) MTR_END("BASE", m);
#else
#define BASE_TRACE()
#define BASE_SCOPE(m)
#define BASE_BEGIN(m)
#define BASE_END(m)
#endif // BASE_TRACING

#endif //SPINDULYS_BASE_H
