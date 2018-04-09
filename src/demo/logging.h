#ifndef PYLONDEMO_LOGGING_H
#define PYLONDEMO_LOGGING_H

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(appLogger)

void initLogging();

#define APP_DEBUG     qCDebug(appLogger)
#define APP_INFO      qCInfo(appLogger)
#define APP_WARN      qCWarning(appLogger)
#define APP_CRITICAL  qCCritical(appLogger)

#endif // PYLONDEMO_LOGGING_H
