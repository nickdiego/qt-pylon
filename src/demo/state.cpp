#include "state.h"

#include "controller.h"
#include "logging.h"

namespace app {

State::State(const char *name, Controller *parent) :
  QState(parent),
  m_controller(parent)
{
  setObjectName(name);
}

State::~State()
{
}

State *State::empty()
{
  static State s_empty;
  return &s_empty;
}

void State::setBusy(bool val)
{
  m_isBusy = val;
  emit busyChanged();
}

void State::onEntry(QEvent *s)
{
  APP_DEBUG << "entering state " << objectName();
  controller().setCurrentState(this);
  QState::onEntry(s);
}

void State::onExit(QEvent *s)
{
  APP_DEBUG << "exiting state " << objectName();
  QState::onExit(s);
}

Controller& State::controller() const
{
  return *m_controller;
}

} // namespace app
