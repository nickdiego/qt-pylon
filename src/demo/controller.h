#ifndef PYLONDEMO_APP_CONTROLLER_H
#define PYLONDEMO_APP_CONTROLLER_H

#include <QImage>
#include <QStateMachine>
#include <QState>

template <typename t>
class QVector;
class PylonCamera;

namespace app {

class State;

static const int kListPageSize = 20;

class Controller : public QStateMachine
{
  Q_OBJECT
  Q_PROPERTY(app::State *currentState READ currentState NOTIFY currentStateChanged)
  Q_PROPERTY(app::State *homeState MEMBER m_homeState CONSTANT)
  Q_PROPERTY(PylonCamera *camera MEMBER m_camera CONSTANT)

public:
  explicit Controller(QObject *parent = nullptr);
  ~Controller();

  State *currentState() const { return m_currentState; }

signals:
  void currentStateChanged();

private slots:
  void onImageCaptured(const QVector<QImage> &images);

private:
  void setupStateMachine();
  void setCurrentState(State *state);
  friend class State;

private:
  State *m_currentState;
  State *m_homeState;
  PylonCamera *m_camera;
};

} // namespace app

#endif //PYLONDEMO_APP_CONTROLLER_H
