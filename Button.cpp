#include "Button.h"
#include "Arduino.h"
#include <functional>

#define PollPeriod 10

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
      this->state = ButtonState::Press;
      this->heldPeriods = 0;
      const ButtonData data = {
        .event = ButtonState::Press,
      };
      this->callback(this->context, data);
    }
    else
    {
      this->state = ButtonState::Release;
      const ButtonData data = {
        .event = this->state,
        .holdTime = this->heldPeriods * PollPeriod,
      };
      this->callback(this->context, data);
    }
  } 
  else if(currentState)
  {
    this->heldPeriods++;
    if( (this->state != ButtonState::Hold) && ((this->heldPeriods*PollPeriod) > holdTime) )
    {
      this->state = ButtonState::Hold;
      const ButtonData data = {
        .event = this->state,
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
  this->holdTime = 1000;
  this->prevState = false;
  this->callback = callback;
  this->context = context;
  this->inverted = inverted;

  pinMode(pin, INPUT_PULLUP);

  Serial.println("Button");
  //auto callback = [this](){this->Poll();};
  //timers->Start(PollPeriod, callback, TimerType::Periodic);
  timers->Start(PollPeriod, TriggerPoll, this, TimerType::Periodic);
}

ButtonState Button::State(void)
{
  return this->state;
}
