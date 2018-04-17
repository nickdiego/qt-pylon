#include "controller.h"

#include <QDateTime>
#include <QString>
#include <QStringBuilder>

#include "pyloncamera.h"

#include "logging.h"

namespace app {

const static char *kStateHome = "home";

Controller::Controller(QObject *parent) :
  QObject(parent),
  m_currentState(kStateHome),
  m_camera(new PylonCamera(this))
{
  connect(m_camera, &PylonCamera::captured, this,
          &Controller::onImageCaptured);

  m_camera->start();
}

Controller::~Controller()
{
}

void Controller::setCurrentState(const QString &state)
{
  m_currentState = state;
  emit currentStateChanged();
}

void Controller::onImageCaptured(const QVector<QImage> &images)
{
    Q_ASSERT(images.size() == 1);

    const char *f = "dd-MM-yyyy_hh-mm-ss-zzz";
    QString suffix = QDateTime::currentDateTime().toString(f);
    QString path = QString("/tmp/pylon-") % suffix % ".jpg";

    images.first().save(path);
    qDebug() << "Captured image saved to '" << path << "'";
    m_camera->start();
}

} // namespace app

