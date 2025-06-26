#ifndef Button_h
#define Button_h

#include "Arduino.h"
#include "Timers.h"

enum class ButtonEvent {
  Press = 0,
  Release = 1,
  Hold = 2,
};

struct ButtonData {
  ButtonEvent event;
  union
  {
    Ticks_t holdTime;
  };
};

typedef void (*ButtonCallback)(void *context, ButtonData data);

class Button {
private:
  int pin;
  Ticks_t holdTime;
  Ticks_t debounceTime;
  ButtonCallback callback;
  void *context;
  bool prevState;
  bool inverted;

public:
  Button(int pin, ButtonCallback callback, void* context, Timers *timers, Ticks_t debounceTime = 50, bool inverted = true);
  void Poll(void);
};

#endif