#ifndef Button_h
#define Button_h

#include "Arduino.h"
#include "Timers.h"

enum class ButtonState {
  Press = 0,
  Release = 1,
  Hold = 2,
};

struct ButtonData {
  ButtonState event;
  union
  {
    Ticks_t holdTime;
    Ticks_t releasedTime;
  };
};

typedef void (*ButtonCallback)(void *context, ButtonData data);

class Button {
private:
  int pin;
  int heldPeriods;
  Ticks_t holdTime;
  ButtonCallback callback;
  void *context;
  bool prevState;
  bool inverted;
  ButtonState state;

public:
  Button(int pin, Timers *timers, ButtonCallback callback = NULL, void* context = NULL, Ticks_t holdTime = 1000, bool inverted = true);
  void Poll(void);
  ButtonData State(void);
  void SetHoldTime(Ticks_t holdTime);
  void Subscribe(ButtonCallback callback = NULL, void* context = NULL);
};

#endif
