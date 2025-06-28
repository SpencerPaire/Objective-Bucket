#ifndef KOTH_H
#define KOTH_H

#include "GameModeRunner.h"

enum KotH_Timers {
  KotH_0,
  KotH_1,
  KotH_2,
  KotH_3,
  KotH_GameTimer,
  KotH_UpdateTimer,
  KotH_FlashTimer,
  KotH_MaxTeams = KotH_3 + 1,
  KotH_MaxGameTimers = KotH_UpdateTimer + 1,
  KotH_MaxTimers = KotH_FlashTimer + 1,
};

#define KOTH_TEAMS 4
#define KOTH_TIMERS (KOTH_TEAMS+1)

class KotH : public GameMode {
  private:
    Timer_t *gameTimers[KotH_MaxTimers];

  public:
    KotH();
    void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
    void DeInit();
    void Pause();
    void Resume();
    void Reset();
    void ButtonEvent(int button, ButtonData data);
    void UpdateScreen();
    bool flash;
};

#endif
