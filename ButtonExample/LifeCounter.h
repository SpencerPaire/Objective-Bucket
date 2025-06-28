#ifndef LIFECOUNTER
#define LIFECOUNTER

#include "GameModeRunner.h"

#define LifeCounter_TEAMS 4

enum LifeCounter_Timers {
  LifeCounter_UpdateTimer,
  LifeCounter_MaxTimers
};

class LifeCounter : public GameMode {
  private:
    Timer_t *gameTimers[LifeCounter_MaxTimers];
    int counts[LifeCounter_TEAMS];

  public:
    LifeCounter();
    void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
    void DeInit();
    void Pause();
    void Resume();
    void Reset();
    void ButtonEvent(int button, ButtonData data);
    void UpdateScreen();
};

#endif
