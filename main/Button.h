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
  Button(int pin, ButtonCallback callback, void* context, Timers *timers, Ticks_t holdTime = 1000, bool inverted = true);
  void Poll(void);
  ButtonState State(void);
};

#endif