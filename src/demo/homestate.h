#ifndef PYLONDEMO_HOMESTATE_H
#define PYLONDEMO_HOMESTATE_H

#include "state.h"

namespace app {

class Controller;

class HomeState : public State
{
  Q_OBJECT

public:
  explicit HomeState(Controller *parent = nullptr);
  virtual ~HomeState();

  virtual void onEntry(QEvent *s) override;
};

} // namespace app

#endif //PYLONDEMO_HOMESTATE_H
