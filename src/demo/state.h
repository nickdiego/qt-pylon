#ifndef PYLONDEMO_APP_STATE_H
#define PYLONDEMO_APP_STATE_H

#include <QStateMachine>
#include <QState>

namespace app {

class Controller;

// Base class for all app states
class State : public QState
{
  Q_OBJECT
  Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)

public:
  explicit State(const char *name = "", Controller *parent = nullptr);
  virtual ~State();
  static State *empty();

  bool isBusy() const { return m_isBusy; }

  virtual void onEntry(QEvent *s) override;
  virtual void onExit(QEvent *s) override;

signals:
  void busyChanged();

protected:
  Controller& controller() const;
  void setBusy(bool val);

private:
  Controller *m_controller;
  bool m_isBusy;
};


} // namespace app

#endif //PYLONDEMO_APP_STATE_H
