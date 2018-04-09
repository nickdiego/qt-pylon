#include "controller.h"

#include "homestate.h"
#include "logging.h"

namespace app {

Controller::Controller(QObject *parent) :
  QStateMachine(parent),
  m_currentState(State::empty()),
  m_homeState(new HomeState(this))
{
  setupStateMachine();
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

} // namespace app

