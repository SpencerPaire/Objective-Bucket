#include "FifthElement.h"

#define GAME_TIME (10 * 1000)

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

void FifthElement::ButtonEvent(int button, ButtonData data)
{
  if(data.event == ButtonState::Release)
  {
      this->timers->Stop(this->gameTimers[FifthElement_GameTimer]);
      this->gameTimers[FifthElement_GameTimer] = this->timers->Start(GAME_TIME, GameOver, this, TimerType::Periodic);
      this->timers->Pause(this->gameTimers[FifthElement_GameTimer]);
  }
  else if(data.event == ButtonState::Press)
  {
    bool allPressed = true;
    for(int i = 0; i <= ButtonEnum::Button3; i++)
    {
      ButtonData otherData = this->buttons[i]->State();
      if (otherData.event == ButtonState::Release)
      {
        allPressed = false;
      }
    }
    if(allPressed)
    {
      this->timers->Resume(this->gameTimers[FifthElement_GameTimer]);
    }
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

void FifthElement::UpdateScreen()
{
  this->lcd->ClearScreen();

  Ticks_t t;
  t = this->timers->RemainingTime(this->gameTimers[FifthElement_GameTimer]);

  bool pressed[ButtonEnum::Button3+1];
  for(int i = 0; i <= ButtonEnum::Button3; i++)
  {
    ButtonData otherData = this->buttons[i]->State();
    if (otherData.event != ButtonState::Release)
    {
      pressed[i] = true;
    }
    else
    {
      pressed[i] = false;
    }
  }

  String msg = "*";

  if(pressed[ButtonEnum::Button0]) { this->lcd->WriteMessage(msg, 0, Alignment::Left, false); }
  if(pressed[ButtonEnum::Button1]) { this->lcd->WriteMessage(msg, 0, Alignment::Right, false); }
  if(pressed[ButtonEnum::Button2]) { this->lcd->WriteMessage(msg, 3, Alignment::Left, false); }
  if(pressed[ButtonEnum::Button3]) { this->lcd->WriteMessage(msg, 3, Alignment::Right, false); }

  msg = TickString(t);
  this->lcd->WriteMessage(msg, 1, Alignment::Center, false);
}

FifthElement::FifthElement()
{
  this->name = "Fifth Element";
}

void FifthElement::Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  Serial.println("FifthElement Selected");
  this->timers = timers;
  this->buttons = buttons;
  this->lcd = lcd;

  this->buttons[ButtonEnum::Button0]->Subscribe(Button0Event, this);
  this->buttons[ButtonEnum::Button1]->Subscribe(Button1Event, this);
  this->buttons[ButtonEnum::Button2]->Subscribe(Button2Event, this);
  this->buttons[ButtonEnum::Button3]->Subscribe(Button3Event, this);

  this->gameTimers[FifthElement_GameTimer] = this->timers->Start(GAME_TIME, GameOver, this,  TimerType::Periodic);
  this->timers->Pause(this->gameTimers[FifthElement_GameTimer]);
  this->gameTimers[FifthElement_UpdateTimer] = this->timers->Start(100, UpdateEvent, this,  TimerType::Periodic);
}

void FifthElement::Pause()
{
  for(int i = 0; i < FifthElement_MaxTimers; i++)
  {
    this->timers->Pause(this->gameTimers[i]);
  }
}

void FifthElement::Resume()
{
  for(int i = 0; i < FifthElement_MaxTimers; i++)
  {
    this->timers->Resume(this->gameTimers[i]);
  }
}

void FifthElement::DeInit()
{
  for(int i = 0; i < FifthElement_MaxTimers; i++)
  {
    this->timers->Stop(this->gameTimers[i]);
  }
}

void FifthElement::Reset()
{
  this->timers->Stop(this->gameTimers[FifthElement_GameTimer]);
  this->gameTimers[FifthElement_GameTimer] = this->timers->Start(GAME_TIME, GameOver, this, TimerType::Periodic);
  this->timers->Pause(this->gameTimers[FifthElement_GameTimer]);
  this->timers->Resume(this->gameTimers[FifthElement_UpdateTimer]);
}
