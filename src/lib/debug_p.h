#ifndef RESTGATE_DEBUG_P_H
#define RESTGATE_DEBUG_P_H

#include <QtCore/qloggingcategory.h>

#define restgateDebug()    qCDebug(restgate).noquote().nospace()
#define restgateInfo()     qCInfo(restgate).noquote().nospace()
#define restgateWarning()  qCWarning(restgate).noquote().nospace()
#define restgateCritical() qCCritical(restgate).noquote().nospace()

Q_DECLARE_LOGGING_CATEGORY(restgate)

#endif // RESTGATE_DEBUG_P_H
