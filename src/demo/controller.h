#ifndef PYLONDEMO_APP_CONTROLLER_H
#define PYLONDEMO_APP_CONTROLLER_H

#include <QImage>

class QString;
template <typename t>
class QVector;
class PylonCamera;

namespace app {

static const int kListPageSize = 20;

class Controller : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString currentState READ currentState NOTIFY currentStateChanged)
  Q_PROPERTY(PylonCamera *camera MEMBER m_camera CONSTANT)

public:
  explicit Controller(QObject *parent = nullptr);
  ~Controller();

  QString currentState() const { return m_currentState; }

signals:
  void currentStateChanged();

private slots:
  void onImageCaptured(const QVector<QImage> &images);

private:
  void setCurrentState(const QString &state);

private:
  QString m_currentState;
  PylonCamera *m_camera;
};

} // namespace app

#endif //PYLONDEMO_APP_CONTROLLER_H
