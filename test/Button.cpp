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
      if(this->callback)
      {
        const ButtonData data = {
          .event = ButtonState::Press,
          .releasedTime = this->heldPeriods * PollPeriod,
        };
        this->callback(this->context, data);
      }
      this->heldPeriods = 0;
    }
    else
    {
      this->state = ButtonState::Release;
      if(this->callback)
      {
        const ButtonData data = {
          .event = this->state,
          .holdTime = this->heldPeriods * PollPeriod,
        };
        this->callback(this->context, data);
      }
      this->heldPeriods = 0;
    }
  } 
  else if(currentState)
  {
    if( (this->state != ButtonState::Hold) && ((this->heldPeriods*PollPeriod) >= holdTime) )
    {
      this->state = ButtonState::Hold;
      if(this->callback)
      {
        const ButtonData data = {
          .event = this->state,
          .holdTime = this->heldPeriods * PollPeriod,
        };
        this->callback(this->context, data);
      }
    }
  }
  this->heldPeriods++;
  this->prevState = currentState;
}

Button::Button(int pin, Timers *timers, ButtonCallback callback, void* context, Ticks_t holdTime, bool inverted)
{
  this->pin = pin;
  this->holdTime = holdTime;
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

ButtonData Button::State(void)
{
  const ButtonData data = {
    .event = this->state,
    .holdTime = this->heldPeriods * PollPeriod,
  };
  return data;
}
