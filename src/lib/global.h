#ifndef RESTGATE_GLOBAL_H
#define RESTGATE_GLOBAL_H

#include <RestGate/config.h>

#ifdef RESTGATE_SHARED
#   ifdef RESTGATE_BUILD
#       define RESTGATE_EXPORT Q_DECL_EXPORT
#   else
#       define RESTGATE_EXPORT Q_DECL_IMPORT
#   endif
#else
#   define RESTGATE_EXPORT
#endif

#define RESTGATE_Q(ClassName) ClassName *q = static_cast<ClassName *>(q_ptr)
#define RESTGATE_D(ClassName) \
    ClassName##Private *d = static_cast<ClassName##Private *>(qGetPtrHelper(d_ptr))

namespace RestGate {
  // Populate !
}

#endif // RESTGATE_GLOBAL_H
