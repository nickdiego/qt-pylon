#include "controller.h"

#include <QDateTime>
#include <QImage>
#include <QStringBuilder>

#include "pyloncamera.h"

#include "homestate.h"
#include "logging.h"

namespace app {

Controller::Controller(QObject *parent) :
  QStateMachine(parent),
  m_currentState(State::empty()),
  m_homeState(new HomeState(this)),
  m_camera(new PylonCamera(this))
{
  setupStateMachine();

  connect(m_camera, &PylonCamera::imageCaptured, this,
          &Controller::onImageCaptured);

}

Controller::~Controller()
{
}

void Controller::setCurrentState(State *state)
{
  m_currentState = state;
  emit currentStateChanged();
}

void Controller::setupStateMachine()
{
  QStateMachine::setInitialState(m_homeState);
}

void Controller::onImageCaptured(const QImage &img)
{
    const char *f = "dd-MM-yyyy_hh-mm-ss-zzz";
    QString suffix = QDateTime::currentDateTime().toString(f);
    QString path = QString("/tmp/pylon-") % suffix % ".jpg";

    img.save(path);
    qDebug() << "Captured image saved to '" << path << "'";
    m_camera->start();
}

} // namespace app

