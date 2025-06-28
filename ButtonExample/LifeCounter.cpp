#include "LifeCounter.h"

static void GameOver(void *context)
{
  GameMode *instance = (GameMode*)context;
  instance->Pause();
}

static void ClearFlash(void *context) {
  LifeCounter *instance = (LifeCounter*)context;
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

void LifeCounter::ButtonEvent(int button, ButtonData data)
{
  if(data.event == ButtonState::Press)
  {
    counts[button]++;
    flash = true;
    this->gameTimers[LifeCounter_FlashTimer] = this->timers->Start(300, ClearFlash, this,  TimerType::OneShot);
  }
}

void LifeCounter::UpdateScreen()
{
  this->lcd->ClearScreen();

  String msg = "  " + String(counts[0]);
  this->lcd->WriteMessage(msg, 0, Alignment::Left, false);

  msg = "  " + String(counts[1]);
  this->lcd->WriteMessage(msg, 3, Alignment::Left, false);

  msg = String(counts[2]) + "  ";
  this->lcd->WriteMessage(msg, 3, Alignment::Right, false);

  msg =String(counts[3]) + "  ";
  this->lcd->WriteMessage(msg, 0, Alignment::Right, false);

  if (flash) {
    msg ="Counted!";
    this->lcd->WriteMessage(msg, 1, Alignment::Center, false);
  }
}

LifeCounter::LifeCounter()
{
  this->name = "Counters";
}

void LifeCounter::Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  Serial.println("Life Counters Selected");
  this->timers = timers;
  this->buttons = buttons;
  this->lcd = lcd;

  this->buttons[ButtonEnum::Button0]->Subscribe(Button0Event, this);
  this->buttons[ButtonEnum::Button1]->Subscribe(Button1Event, this);
  this->buttons[ButtonEnum::Button2]->Subscribe(Button2Event, this);
  this->buttons[ButtonEnum::Button3]->Subscribe(Button3Event, this);

  this->gameTimers[LifeCounter_UpdateTimer] = this->timers->Start(100, UpdateEvent, this,  TimerType::Periodic);
  Reset();
}

void LifeCounter::Pause()
{
}

void LifeCounter::Resume()
{
}

void LifeCounter::DeInit()
{
  for(int i = 0; i < LifeCounter_MaxTimers; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
  }
}

void LifeCounter::Reset()
{
  for(int i = 0; i < LifeCounter_TEAMS; i++)
  {
    counts[i] = 0;
  }
}
