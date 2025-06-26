#include "GameModeRunner.h"
#include "Arduino.h"
#include <functional>

#define ResetHoldTime 2000

static void Button0Event(void* context, ButtonData data)
{
  GameModeRunner *runner = (GameModeRunner*)context;
  runner->ButtonEvent(ButtonEnum::Button0, data);
}

static void Button1Event(void* context, ButtonData data)
{
  GameModeRunner *runner = (GameModeRunner*)context;
  runner->ButtonEvent(ButtonEnum::Button1, data);
}

static void Button2Event(void* context, ButtonData data)
{
  GameModeRunner *runner = (GameModeRunner*)context;
  runner->ButtonEvent(ButtonEnum::Button2, data);
}

static void Button3Event(void* context, ButtonData data)
{
  GameModeRunner *runner = (GameModeRunner*)context;
  runner->ButtonEvent(ButtonEnum::Button3, data);
}

static void ResetButton(void* context, ButtonData data)
{
  GameModeRunner *runner = (GameModeRunner*)context;
  runner->ButtonEvent(ButtonEnum::ButtonReset, data);
}

void GameModeRunner::ButtonEvent(int button, ButtonData data)
{
  if(button == ButtonEnum::ButtonReset)
  {
    if(data.event == ButtonState::Release)
    {
      if(data.holdTime < 1000)
      {
        Serial.println("Reset Game");
        if(this->running == true)
        {
          this->games[this->mode]->Reset();
        }
      }
    }
    else if(data.event == ButtonState::Hold)
    {
      Serial.println("Return to Game Select");
      if(this->running == true)
      {
        this->games[this->mode]->DeInit();
        this->running = false;
        this->SetButtons();
        this->GameSelect();
      }
    }
  }
  else if(button == ButtonEnum::Button0)
  {
    if(data.event == ButtonState::Release)
    {
      Serial.println("Up");
      this->mode = (this->mode + 1) % this->numGames;
      this->GameSelect();
    }
  }
  else if(button == ButtonEnum::Button1)
  {
    if(data.event == ButtonState::Release)
    {
      Serial.println("Down");
      this->mode = (this->mode - 1) % this->numGames;
      this->GameSelect();
    }
  }
  else if(button == ButtonEnum::Button2)
  {
    if(data.event == ButtonState::Release)
    {
      Serial.println("Select Game");
      this->games[this->mode]->Init(this->timers, this->buttons, this->lcd);
      this->running = true;
    }
  }
}

void GameModeRunner::SetButtons()
{
  this->buttons[ButtonEnum::Button0]->Subscribe(Button0Event, this);
  this->buttons[ButtonEnum::Button1]->Subscribe(Button1Event, this);
  this->buttons[ButtonEnum::Button2]->Subscribe(Button2Event, this);
  this->buttons[ButtonEnum::Button3]->Subscribe(Button3Event, this);
}

void GameModeRunner::GameSelect()
{
  this->lcd->ClearScreen();

  String msg;
  if(this->numGames == 0)
  {
    msg = "No Games Loaded";
    this->lcd->WriteMessage(msg, 1, 1);
  }
  else
  {
    if(this->mode == GAMEMODE_IDLE) { this->mode = 0; }

    if(this->numGames >= this->lcd->GetRows())
    {
      int prev = (this->mode == 0) ? this->numGames - 1 : this->mode - 1;
      msg = this->games[prev]->GetName();
      this->lcd->WriteMessage(msg, 0, 1);
    }

    int numberOfLines = min(this->lcd->GetRows() - 1, this->numGames);
    for(int i = 0; i < numberOfLines; i++)
    {
      int game = (this->mode + i) % this->numGames;
      msg = this->games[game]->GetName();
      this->lcd->WriteMessage(msg, i+1, 1);
    }

    msg = "*";
    this->lcd->WriteMessageWithoutClear(msg, 1, 0);
  }
}

GameModeRunner::GameModeRunner(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  this->Init(timers, buttons, lcd);
}

void GameModeRunner::Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  this->running = false;
  this->mode = GAMEMODE_IDLE;
  this->numGames = 0;
  this->timers = timers;
  this->buttons = buttons;
  this->lcd = lcd;

  if(this->timers != NULL && buttons != NULL && lcd != NULL)
  {
    this->buttons[ButtonEnum::ButtonReset]->Subscribe(ResetButton, this);
    this->buttons[ButtonEnum::ButtonReset]->SetHoldTime(ResetHoldTime);

    this->SetButtons();
    this->GameSelect();
  }
}

int GameModeRunner::CurrentMode()
{
  return this->mode;
}

void GameModeRunner::AddGameMode(GameMode *game)
{
  this->games[this->numGames++] = game;
  this->GameSelect();
}


GameMode::GameMode()
{
  this->name = "Default GameMode";
}

String GameMode::GetName()
{
  return this->name;
}

void GameMode::Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd)
{
  Serial.println("Base GameMode Selected");
}

void GameMode::DeInit()
{
  
}

void GameMode::Reset()
{

}

void GameMode::ButtonEvent(int button, ButtonData data)
{

}

void GameMode::UpdateScreen()
{

}
