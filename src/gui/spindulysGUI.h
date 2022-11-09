#ifndef SPINDULYS_GUI_H
#define SPINDULYS_GUI_H

#include <spindulys.h>


#if SPINDULYS_USE_NAMESPACES


#define GUI_NAMESPACE_OPEN_SCOPE SPINDULYS_NAMESPACE_OPEN_SCOPE SPINDULYS_BASE_NAMESPACE_OPEN_SCOPE SPINDULYS_BACKEND_CPU_NAMESPACE_OPEN_SCOPE SPINDULYS_GUI_NAMESPACE_OPEN_SCOPE
#define GUI_NAMESPACE_CLOSE_SCOPE SPINDULYS_NAMESPACE_CLOSE_SCOPE SPINDULYS_BASE_NAMESPACE_CLOSE_SCOPE SPINDULYS_GUI_NAMESPACE_CLOSE_SCOPE SPINDULYS_GUI_NAMESPACE_CLOSE_SCOPE

#else

#define GUI_NAMESPACE_OPEN_SCOPE 
#define GUI_NAMESPACE_CLOSE_SCOPE 

#endif // SPINDULYS_USE_NAMESPACE

// Set to 0 to disable GUI tracing.
#define GUI_TRACING 0
#if GUI_TRACING
#define GUI_TRACE() MTR_SCOPE("GUI", __func__);
#define GUI_SCOPE(m) MTR_SCOPE("GUI", m);
#else
#define GUI_TRACE()
#define GUI_SCOPE(m)
#endif // GUI_TRACING

#endif //SPINDULYS_GUI_H
