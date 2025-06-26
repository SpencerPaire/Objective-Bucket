#include "Button.h"
#include "Arduino.h"
#include <functional>


static void TriggerPoll(void* context)
{
  Button *button = (Button*)context;
  button->Poll();
}

void Button::Poll(void)
{
  bool currentState = digitalRead(this->pin);
  if(this->inverted) { currentState = !currentState; }

  if(this->prevState != currentState)
  {
    if(currentState)
    {
      const ButtonData data = {
        .event = ButtonEvent::Press,
      };
      this->callback(this->context, data);
    }
    else
    {
      const ButtonData data = {
        .event = ButtonEvent::Release,
      };
      this->callback(this->context, data);
    }
  } 
  this->prevState = currentState;
}

Button::Button(int pin, ButtonCallback callback, void* context, Timers *timers, Ticks_t debounceTime, bool inverted)
{
  this->pin = pin;
  this->debounceTime = debounceTime;
  this->holdTime = 0;
  this->prevState = false;
  this->callback = callback;
  this->context = context;
  this->inverted = inverted;

  pinMode(pin, INPUT_PULLUP);

  Serial.println("Button");
  //auto callback = [this](){this->Poll();};
  //timers->Start(1000, callback, TimerType::Periodic);
  timers->Start(10, TriggerPoll, this, TimerType::Periodic);
}
