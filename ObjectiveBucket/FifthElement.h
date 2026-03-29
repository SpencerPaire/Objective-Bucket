#pragma once
#include "GameModeRunner.h"

// ============================================================
//  Fifth Element
//  - All 4 buttons must be held simultaneously to start timer
//  - Releasing any button pauses the timer
//  - Timer completes = game over (Pause called)
//  - Configurable: hold duration to win, update rate
// ============================================================

enum FifthElement_Timers {
  FifthElement_GameTimer,
  FifthElement_UpdateTimer,
  FifthElement_MaxTimers,
};

#define FIFTH_DEFAULT_HOLD_MS  (10 * 1000)   // 10 seconds

class FifthElement : public GameMode {

  // ---- static callbacks ----
  static void sGameOver(void *ctx) { ((GameMode*)ctx)->Pause(); }
  static void sUpdate(void *ctx)   { ((GameMode*)ctx)->UpdateScreen(); }
  static void sBtn0(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button0, d); }
  static void sBtn1(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button1, d); }
  static void sBtn2(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button2, d); }
  static void sBtn3(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button3, d); }

  // ---- private state ----
  Timer_t *gameTimers[FifthElement_MaxTimers];
  long holdMs;

  static String TickString(Ticks_t ticks) {
    ticks /= 100;
    uint8_t tenths  = ticks % 10; ticks /= 10;
    uint8_t seconds = ticks % 60;
    uint8_t minutes = ticks / 60;
    char buf[8];
    sprintf(buf, "%02d:%02d.%d", minutes, seconds, tenths);
    return buf;
  }

public:
  FifthElement() {
    this->name = "Fifth Element";
    holdMs     = FIFTH_DEFAULT_HOLD_MS;

    this->numConfigs = 1;
    this->configs[0] = { "holdTime", "Hold Duration", FIFTH_DEFAULT_HOLD_MS / 1000, 3, 60, "sec" };
  }

  void SetConfig(String key, long value) override {
    if (key == "holdTime") {
      holdMs = value * 1000;
      configs[0].value = value;
    }
  }

  void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd) override {
    Serial.println("FifthElement Selected");
    this->timers  = timers;
    this->buttons = buttons;
    this->lcd     = lcd;

    this->buttons[ButtonEnum::Button0]->Subscribe(sBtn0, this);
    this->buttons[ButtonEnum::Button1]->Subscribe(sBtn1, this);
    this->buttons[ButtonEnum::Button2]->Subscribe(sBtn2, this);
    this->buttons[ButtonEnum::Button3]->Subscribe(sBtn3, this);

    gameTimers[FifthElement_GameTimer]   = timers->Start(holdMs, sGameOver, this, TimerType::Periodic);
    timers->Pause(gameTimers[FifthElement_GameTimer]);
    gameTimers[FifthElement_UpdateTimer] = timers->Start(100, sUpdate, this, TimerType::Periodic);
  }

  void ButtonEvent(int button, ButtonData data) override {
    if (data.event == ButtonState::Release) {
      // Any button released — stop and reset the game timer
      timers->Stop(gameTimers[FifthElement_GameTimer]);
      gameTimers[FifthElement_GameTimer] = timers->Start(holdMs, sGameOver, this, TimerType::Periodic);
      timers->Pause(gameTimers[FifthElement_GameTimer]);
    }
    else if (data.event == ButtonState::Press) {
      // Check if all buttons are now held
      bool allPressed = true;
      for (int i = 0; i <= ButtonEnum::Button3; i++) {
        if (this->buttons[i]->State().event == ButtonState::Release) {
          allPressed = false;
          break;
        }
      }
      if (allPressed) timers->Resume(gameTimers[FifthElement_GameTimer]);
    }
  }

  void UpdateScreen() override {
    lcd->ClearScreen();

    Ticks_t t = timers->RemainingTime(gameTimers[FifthElement_GameTimer]);

    bool pressed[ButtonEnum::Button3 + 1];
    for (int i = 0; i <= ButtonEnum::Button3; i++) {
      pressed[i] = (this->buttons[i]->State().event != ButtonState::Release);
    }

    String star = "*";
    if (pressed[ButtonEnum::Button0]) lcd->WriteMessage(star, 0, Alignment::Left,  false);
    if (pressed[ButtonEnum::Button1]) lcd->WriteMessage(star, 0, Alignment::Right, false);
    if (pressed[ButtonEnum::Button2]) lcd->WriteMessage(star, 3, Alignment::Left,  false);
    if (pressed[ButtonEnum::Button3]) lcd->WriteMessage(star, 3, Alignment::Right, false);

    lcd->WriteMessage(TickString(t), 1, Alignment::Center, false);
  }

  void Pause() override {
    for (int i = 0; i < FifthElement_MaxTimers; i++) timers->Pause(gameTimers[i]);
  }

  void Resume() override {
    for (int i = 0; i < FifthElement_MaxTimers; i++) timers->Resume(gameTimers[i]);
  }

  void DeInit() override {
    for (int i = 0; i < FifthElement_MaxTimers; i++) timers->Stop(gameTimers[i]);
  }

  void Reset() override {
    timers->Stop(gameTimers[FifthElement_GameTimer]);
    gameTimers[FifthElement_GameTimer] = timers->Start(holdMs, sGameOver, this, TimerType::Periodic);
    timers->Pause(gameTimers[FifthElement_GameTimer]);
    timers->Resume(gameTimers[FifthElement_UpdateTimer]);
  }
};
