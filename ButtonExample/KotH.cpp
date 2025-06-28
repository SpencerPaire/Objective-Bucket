#include "KotH.h"

#define GAME_TIME (10 * 60 * 1000)

static void GameOver(void *context)
{
  GameMode *instance = (GameMode*)context;
  instance->Pause();
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
      for(int i = 0; i < KotH_MaxTeams; i++)
      {
        this->timers->Pause(this->gameTimers[i]);
      }
      this->timers->Resume(this->gameTimers[button]);
      this->timers->Resume(this->gameTimers[KotH_GameTimer]);
  }
}

static String TickString(Ticks_t ticks)
{
  ticks = ticks / 100;
  uint8_t tenths = ticks % 10;
  ticks = ticks / 10;
  uint8_t seconds = ticks % 60;
  uint8_t minutes = ticks / 60;

  char msg[] = "00:00.0";

  sprintf(msg, "%02d:%02d.%d", minutes, seconds, tenths);

  return msg;
}

void KotH::UpdateScreen()
{
  this->lcd->ClearScreen();

  Ticks_t t[KotH_MaxGameTimers];
  for(int i = 0; i < KotH_MaxTeams; i++)
  {
    t[i] =  this->timers->ElapsedTime(this->gameTimers[i]);
  }
  t[KotH_GameTimer] = this->timers->RemainingTime(this->gameTimers[KotH_GameTimer]);

  String msg = TickString(t[0]);
  this->lcd->WriteMessage(msg, 0, Alignment::Left, false);

  msg = TickString(t[1]);
  this->lcd->WriteMessage(msg, 0, Alignment::Right, false);

  msg = TickString(t[2]);
  this->lcd->WriteMessage(msg, 3, Alignment::Left, false);

  msg = TickString(t[3]);
  this->lcd->WriteMessage(msg, 3, Alignment::Right, false);

  msg = TickString(t[KotH_GameTimer]);
  this->lcd->WriteMessage(msg, 1, Alignment::Center, false);
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

  for(int i = 0; i < KotH_MaxGameTimers; i++)
  {
    this->gameTimers[i] = this->timers->Start(GAME_TIME, GameOver, this, TimerType::Periodic);
    this->timers->Pause(this->gameTimers[i]);
  }
  this->gameTimers[KotH_UpdateTimer] = this->timers->Start(100, UpdateEvent, this,  TimerType::Periodic);
}

void KotH::Pause()
{
  for(int i = 0; i < KotH_MaxTimers; i++)
  {
    this->timers->Pause(this->gameTimers[i]);
  }
}

void KotH::Resume()
{
  for(int i = 0; i < KotH_MaxTimers; i++)
  {
    this->timers->Resume(this->gameTimers[i]);
  }
}

void KotH::DeInit()
{
  for(int i = 0; i < KotH_MaxTimers; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
  }
}

void KotH::Reset()
{
  for(int i = 0; i < KotH_MaxGameTimers; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
    this->gameTimers[i] = this->timers->Start(GAME_TIME, GameOver, this, TimerType::Periodic);
    this->timers->Pause(this->gameTimers[i]);
  }
  this->timers->Resume(this->gameTimers[KotH_UpdateTimer]);
  //this->gameTimers[KotH_UpdateTimer] = this->timers->Start(500, UpdateEvent, this,  TimerType::Periodic);
}
