#ifndef KOTH_H
#define KOTH_H

#include "GameModeRunner.h"

#define KOTH_TEAMS 4
#define KOTH_TIMERS (KOTH_TEAMS+1)

class KotH : public GameMode {
  private:
    Timer_t *gameTimers[KOTH_TIMERS];

  public:
    KotH();
    void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
    void DeInit();
    void Reset();
    void ButtonEvent(int button, ButtonData data);
    void UpdateScreen();
};

#endif