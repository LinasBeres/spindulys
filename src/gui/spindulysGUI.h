#ifndef SPINDULYS_GUI_H
#define SPINDULYS_GUI_H

#include <spindulys.h>


#if SPINDULYS_USE_NAMESPACES


#define GUI_NAMESPACE_OPEN_SCOPE SPINDULYS_NAMESPACE_OPEN_SCOPE SPINDULYS_FRONTEND_NAMESPACE_OPEN_SCOPE SPINDULYS_BACKEND_CPU_NAMESPACE_OPEN_SCOPE SPINDULYS_GUI_NAMESPACE_OPEN_SCOPE
#define GUI_NAMESPACE_CLOSE_SCOPE SPINDULYS_NAMESPACE_CLOSE_SCOPE SPINDULYS_FRONTEND_NAMESPACE_CLOSE_SCOPE SPINDULYS_GUI_NAMESPACE_CLOSE_SCOPE SPINDULYS_GUI_NAMESPACE_CLOSE_SCOPE

#else

#define GUI_NAMESPACE_OPEN_SCOPE 
#define GUI_NAMESPACE_CLOSE_SCOPE 

#endif // SPINDULYS_USE_NAMESPACE

#endif //SPINDULYS_GUI_H
