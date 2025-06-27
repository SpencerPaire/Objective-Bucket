#include "KotH.h"

#define GAME_TIME (10 * 60 * 1000)

static void GameOver(void *context)
{
  // Pause all timers
  // Display Screen
}

static void UpdateEvent(void *context)
{
  GameMode *instance = (GameMode*)context;
  instance->UpdateScreen();
}

static void Button0Event(void* context, ButtonData data)
{
  GameMode *instance = (GameMode*)context;
  instance->ButtonEvent(ButtonEnum::Button0, data);
}

static void Button1Event(void* context, ButtonData data)
{
  GameMode *instance = (GameMode*)context;
  instance->ButtonEvent(ButtonEnum::Button1, data);
}

static void Button2Event(void* context, ButtonData data)
{
  GameMode *instance = (GameMode*)context;
  instance->ButtonEvent(ButtonEnum::Button2, data);
}

static void Button3Event(void* context, ButtonData data)
{
  GameMode *instance = (GameMode*)context;
  instance->ButtonEvent(ButtonEnum::Button3, data);
}

void KotH::ButtonEvent(int button, ButtonData data)
{
  if(data.event == ButtonState::Release)
  {
      for(int i = 0; i < KOTH_TEAMS; i++)
      {
        this->timers->Pause(this->gameTimers[i]);
      }
      this->timers->Resume(this->gameTimers[button]);
  }
}

void KotH::UpdateScreen()
{
  Ticks_t b0Time = this->timers->ElapsedTime(this->gameTimers[0]);
  Serial.printf("B0 Time: %d ms\n", b0Time);

  //this->lcd->ClearScreen();
}

KotH::KotH()
{
  this->name = "King of the Hill";
}

void KotH::Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  Serial.println("KotH Selected");
  this->timers = timers;
  this->buttons = buttons;
  this->lcd = lcd;

  this->buttons[ButtonEnum::Button0]->Subscribe(Button0Event, this);
  this->buttons[ButtonEnum::Button1]->Subscribe(Button1Event, this);
  this->buttons[ButtonEnum::Button2]->Subscribe(Button2Event, this);
  this->buttons[ButtonEnum::Button3]->Subscribe(Button3Event, this);

  for(int i = 0; i < KOTH_TEAMS; i++)
  {
    this->gameTimers[i] = this->timers->Start(GAME_TIME, GameOver);
    this->timers->Pause(this->gameTimers[i]);
  }

  this->timers->Start(1000, UpdateEvent, this,  TimerType::Periodic);
}

void KotH::DeInit()
{
  for(int i = 0; i < KOTH_TIMERS; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
  }
}

void KotH::Reset()
{
  for(int i = 0; i < KOTH_TEAMS; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
    this->gameTimers[i] = this->timers->Start(GAME_TIME, GameOver);
    this->timers->Pause(this->gameTimers[i]);
  }
}