#ifndef FIFTHELEMENT_H
#define FIFTHELEMENT_H

#include "GameModeRunner.h"

enum FifthElement_Timers {
  FifthElement_GameTimer,
  FifthElement_UpdateTimer,
  FifthElement_MaxTimers,
};

class FifthElement : public GameMode {
  private:
    Timer_t *gameTimers[FifthElement_MaxTimers];

  public:
    FifthElement();
    void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
    void DeInit();
    void Pause();
    void Resume();
    void Reset();
    void ButtonEvent(int button, ButtonData data);
    void UpdateScreen();
};

#endif