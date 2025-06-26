#include "Timers.h"
#include "Arduino.h"

Timers::Timers()
{
  this->lastTime = millis();
  this->start = NULL;
}

void Timers::Run()
{
  Ticks_t currentTime = millis();
  Ticks_t elapsedTime = currentTime - this->lastTime;
  this->lastTime = currentTime;

  Timer_t *current = this->start;
  while(current != NULL)
  {
    if(current->running)
    {
      if(current->remainingTicks > elapsedTime)
      {
        current->remainingTicks -= elapsedTime; 
      }
      else
      {
        current->remainingTicks = (current->reload) ? current->initialTicks : 0;
        if(current->lambdaCallback)
        {
          //Serial.println("Calling Lambda callback");
          (*current->lambda)();
        }
        else
        {
          //Serial.println("Calling c callback");
          current->callback(current->context);
        }
      }
    }
    current = current->next;
  }
  
  Timer_t *prev = NULL;
  current = this->start;
  while(current != NULL)
  {
    Timer_t *next = current->next;
    if(current->remainingTicks == 0 && !current->reload)
    {
      if(prev != NULL)
      {
        prev->next = current->next;
      }
      else
      {
        this->start = current->next;
      }
      free(current);
    }
    else
    {
      prev = current;
    }
    current = next;
  }
}

Timer_t *Timers::Start(Ticks_t ticks, TimerCallback callback, void *context, TimerType type)
{
  Timer_t *timer = (Timer_t*)malloc(sizeof(Timer_t));
  timer->initialTicks = ticks;
  timer->remainingTicks = ticks;
  timer->lambdaCallback = false;
  timer->callback = callback;
  timer->context = context;
  timer->running = true;
  timer->reload = (type==TimerType::Periodic);
  timer->next = this->start;
  this->start = timer;

  Serial.println("StartTimer");
  
  return timer;
}

Timer_t *Timers::Start(Ticks_t ticks, std::function<void(void)> lambda, TimerType type)
{
  Timer_t *timer = (Timer_t*)malloc(sizeof(Timer_t));
  timer->initialTicks = ticks;
  timer->remainingTicks = ticks;
  timer->lambdaCallback = true;
  timer->lambda = &lambda;
  timer->running = true;
  timer->reload = (type==TimerType::Periodic);
  timer->next = this->start;
  this->start = timer;

  Serial.println("StartTimer");
  
  return timer;
}

void Timers::Stop(Timer_t *timer)
{
  timer->remainingTicks = 0;
  timer->reload = false;
}

void Timers::Pause(Timer_t *timer)
{
  timer->running = false;
}

void Timers::Resume(Timer_t *timer)
{
  timer->running = true;
}

Ticks_t Timers::ElapsedTime(Timer_t *timer)
{
  return timer->initialTicks - timer->remainingTicks;
}
