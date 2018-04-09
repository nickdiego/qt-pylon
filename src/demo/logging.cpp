#include "logging.h"

#ifdef ENABLE_VERBOSE_LOG
# define APP_LOG_SEVERITY QtDebugMsg
#else
# define APP_LOG_SEVERITY QtInfoMsg
#endif

Q_LOGGING_CATEGORY(appLogger, "app", APP_LOG_SEVERITY)

void initLogging() {
  qSetMessagePattern("### %{appname} [%{time}][%{category}](%{type})%{function}(): %{message}");
}
