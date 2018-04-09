#include "homestate.h"

#include <QList>

#include "controller.h"
#include "logging.h"

namespace app {

HomeState::HomeState(Controller *parent) :
  State("home", parent)
{
}

HomeState::~HomeState()
{
}

void HomeState::onEntry(QEvent *s)
{
  State::onEntry(s);
}

} // namespace app

