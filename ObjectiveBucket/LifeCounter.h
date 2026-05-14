#pragma once
#include "GameModeRunner.h"

// ============================================================
//  Life / Hit Counter
//  - Each button increments that team's counter
//  - Optional countdown timer; 0 = no timer
//  - Configurable: starting count, match time
// ============================================================

#define LIFECOUNTER_TEAMS 4

enum LifeCounter_Timers {
  LifeCounter_UpdateTimer,
  LifeCounter_FlashTimer,
  LifeCounter_GameTimer,
  LifeCounter_MaxTimers,
};

#define LIFE_DEFAULT_START_COUNT  0
#define LIFE_DEFAULT_MATCH_SEC    0     // 0 = no timer by default

class LifeCounter : public GameMode {

  // ---- static callbacks ----
  static void sGameOver(void *ctx)   { ((LifeCounter*)ctx)->gameOver = true; ((GameMode*)ctx)->Pause(); }
  static void sClearFlash(void *ctx) { ((LifeCounter*)ctx)->flash = false; }
  static void sUpdate(void *ctx)     { ((GameMode*)ctx)->UpdateScreen(); }
  static void sBtn0(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button0, d); }
  static void sBtn1(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button1, d); }
  static void sBtn2(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button2, d); }
  static void sBtn3(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button3, d); }

  // ---- private state ----
  Timer_t *gameTimers[LifeCounter_MaxTimers];
  int counts[LIFECOUNTER_TEAMS];
  int startCount;
  long matchSeconds;

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
  bool flash    = false;
  bool gameOver = false;

  bool IsGameOver() override { return gameOver; }

  long GetRemainingMs() override {
    if (matchSeconds <= 0) return -1; // no timer
    return timers->RemainingTime(gameTimers[LifeCounter_GameTimer]);
  }

  LifeCounter() {
    this->name  = "Counters";
    startCount  = LIFE_DEFAULT_START_COUNT;
    matchSeconds = LIFE_DEFAULT_MATCH_SEC;

    this->numConfigs = 2;
    this->configs[0] = { "matchTime",  "Match Time",     LIFE_DEFAULT_MATCH_SEC,    0, 600, "sec" };
    this->configs[1] = { "startCount", "Starting Count", LIFE_DEFAULT_START_COUNT,  0, 20,  "hits" };
  }

  void SetConfig(String key, long value) override {
    if (key == "matchTime")  { matchSeconds = value; configs[0].value = value; }
    if (key == "startCount") { startCount   = value; configs[1].value = value; }
  }

  void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd) override {
    Serial.println("Life Counters Selected");
    this->timers  = timers;
    this->buttons = buttons;
    this->lcd     = lcd;
    this->gameOver = false;

    this->buttons[ButtonEnum::Button0]->Subscribe(sBtn0, this);
    this->buttons[ButtonEnum::Button1]->Subscribe(sBtn1, this);
    this->buttons[ButtonEnum::Button2]->Subscribe(sBtn2, this);
    this->buttons[ButtonEnum::Button3]->Subscribe(sBtn3, this);

    gameTimers[LifeCounter_UpdateTimer] = timers->Start(100, sUpdate, this, TimerType::Periodic);

    if (matchSeconds > 0) {
      Ticks_t gameTicks = (Ticks_t)matchSeconds * 1000;
      gameTimers[LifeCounter_GameTimer] = timers->Start(gameTicks, sGameOver, this, TimerType::Periodic);
    }

    Reset();
  }

  void ButtonEvent(int button, ButtonData data) override {
    if (data.event == ButtonState::Press) {
      counts[button]++;
      flash = true;
      gameTimers[LifeCounter_FlashTimer] = timers->Start(300, sClearFlash, this, TimerType::OneShot);
    }
  }

  void UpdateScreen() override {
    lcd->ClearScreen();

    lcd->WriteMessage("  " + String(counts[0]), 0, Alignment::Left,  false);
    lcd->WriteMessage(String(counts[3]) + "  ", 0, Alignment::Right, false);
    lcd->WriteMessage("  " + String(counts[1]), 3, Alignment::Left,  false);
    lcd->WriteMessage(String(counts[2]) + "  ", 3, Alignment::Right, false);

    if (flash) {
      lcd->WriteMessage("Counted!", 1, Alignment::Center, false);
    }

    if (matchSeconds > 0) {
      Ticks_t t = timers->RemainingTime(gameTimers[LifeCounter_GameTimer]);
      lcd->WriteMessage(TickString(t), 2, Alignment::Center, false);
    }
  }

  void Pause() override {
    timers->Pause(gameTimers[LifeCounter_UpdateTimer]);
    if (matchSeconds > 0) timers->Pause(gameTimers[LifeCounter_GameTimer]);
  }

  void Resume() override {
    timers->Resume(gameTimers[LifeCounter_UpdateTimer]);
    if (matchSeconds > 0) timers->Resume(gameTimers[LifeCounter_GameTimer]);
  }

  void DeInit() override {
    timers->Stop(gameTimers[LifeCounter_UpdateTimer]);
    if (matchSeconds > 0) timers->Stop(gameTimers[LifeCounter_GameTimer]);
  }

  void Reset() override {
    gameOver = false;
    for (int i = 0; i < LIFECOUNTER_TEAMS; i++) counts[i] = startCount;
    if (matchSeconds > 0) {
      timers->Stop(gameTimers[LifeCounter_GameTimer]);
      Ticks_t gameTicks = (Ticks_t)matchSeconds * 1000;
      gameTimers[LifeCounter_GameTimer] = timers->Start(gameTicks, sGameOver, this, TimerType::Periodic);
    }
  }
};