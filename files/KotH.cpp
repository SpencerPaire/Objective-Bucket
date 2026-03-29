#include "KotH.h"

static void GameOver(void *context)
{
  GameMode *instance = (GameMode*)context;
  instance->Pause();
}

static void ClearFlash(void *context) {
  KotH *instance = (KotH*)context;
  instance->flash = false;
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
      this->gameTimers[KotH_FlashTimer] = this->timers->Start(500, ClearFlash, this, TimerType::OneShot);
      flash = true;
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
    t[i] = this->timers->ElapsedTime(this->gameTimers[i]);
  }
  t[KotH_GameTimer] = this->timers->RemainingTime(this->gameTimers[KotH_GameTimer]);

  this->lcd->WriteMessage(TickString(t[0]), 0, Alignment::Left, false);
  this->lcd->WriteMessage(TickString(t[1]), 3, Alignment::Left, false);
  this->lcd->WriteMessage(TickString(t[2]), 3, Alignment::Right, false);
  this->lcd->WriteMessage(TickString(t[3]), 0, Alignment::Right, false);
  this->lcd->WriteMessage(TickString(t[KotH_GameTimer]), 1, Alignment::Center, false);

  if (flash) {
    this->lcd->WriteMessage("Captured!", 2, Alignment::Center, false);
  }
}

KotH::KotH()
{
  this->name = "King of the Hill";
  this->matchSeconds = KOTH_DEFAULT_TIME;
  this->flash = false;

  // Expose match time as a configurable setting
  this->numConfigs = 1;
  this->configs[0] = { "matchTime", "Match Time", KOTH_DEFAULT_TIME, 60, 30 * 60, "sec" };
}

void KotH::SetConfig(String key, long value)
{
  if(key == "matchTime")
  {
    this->matchSeconds = value;
    this->configs[0].value = value;
  }
}

void KotH::Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  Serial.println("KotH Selected");
  this->timers = timers;
  this->buttons = buttons;
  this->lcd = lcd;
  this->flash = false;

  this->buttons[ButtonEnum::Button0]->Subscribe(Button0Event, this);
  this->buttons[ButtonEnum::Button1]->Subscribe(Button1Event, this);
  this->buttons[ButtonEnum::Button2]->Subscribe(Button2Event, this);
  this->buttons[ButtonEnum::Button3]->Subscribe(Button3Event, this);

  Ticks_t gameTicks = (Ticks_t)this->matchSeconds * 1000;
  for(int i = 0; i < KotH_MaxGameTimers; i++)
  {
    this->gameTimers[i] = this->timers->Start(gameTicks, GameOver, this, TimerType::Periodic);
    this->timers->Pause(this->gameTimers[i]);
  }
  this->gameTimers[KotH_UpdateTimer] = this->timers->Start(100, UpdateEvent, this, TimerType::Periodic);
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
  Ticks_t gameTicks = (Ticks_t)this->matchSeconds * 1000;
  for(int i = 0; i < KotH_MaxGameTimers; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
    this->gameTimers[i] = this->timers->Start(gameTicks, GameOver, this, TimerType::Periodic);
    this->timers->Pause(this->gameTimers[i]);
  }
  this->timers->Resume(this->gameTimers[KotH_UpdateTimer]);
}
