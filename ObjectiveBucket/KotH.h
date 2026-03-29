#pragma once
#include "GameModeRunner.h"

// ============================================================
//  King of the Hill
//  - 4 team elapsed timers (one per button)
//  - 1 match countdown timer
//  - Pressing a button pauses all others and resumes that team
//  - Configurable: match duration, capture flash duration
// ============================================================

enum KotH_Timers {
  KotH_0,
  KotH_1,
  KotH_2,
  KotH_3,
  KotH_GameTimer,
  KotH_UpdateTimer,
  KotH_FlashTimer,
  KotH_MaxTeams      = KotH_3 + 1,
  KotH_MaxGameTimers = KotH_GameTimer + 1,
  KotH_MaxTimers     = KotH_UpdateTimer + 1,
};

#define KOTH_DEFAULT_MATCH_SEC  (6 * 60)
#define KOTH_DEFAULT_FLASH_MS   500

class KotH : public GameMode {

  // ---- static callbacks ----
  static void sClearFlash(void *ctx) { ((KotH*)ctx)->flash = false; }
  static void sUpdate(void *ctx)     { ((GameMode*)ctx)->UpdateScreen(); }
  static void sGameOver(void *ctx)   { ((GameMode*)ctx)->Pause(); }
  static void sBtn0(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button0, d); }
  static void sBtn1(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button1, d); }
  static void sBtn2(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button2, d); }
  static void sBtn3(void *ctx, ButtonData d) { ((GameMode*)ctx)->ButtonEvent(ButtonEnum::Button3, d); }

  // ---- private state ----
  Timer_t *gameTimers[KotH_MaxTimers];
  long matchSeconds;
  long flashMs;

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
  bool flash = false;

  KotH() {
    this->name   = "King of the Hill";
    matchSeconds = KOTH_DEFAULT_MATCH_SEC;
    flashMs      = KOTH_DEFAULT_FLASH_MS;

    this->numConfigs = 2;
    this->configs[0] = { "matchTime",  "Match Time",     KOTH_DEFAULT_MATCH_SEC, 60,  30*60, "sec" };
    this->configs[1] = { "flashTime",  "Capture Flash",  KOTH_DEFAULT_FLASH_MS,  100, 2000,  "ms"  };
  }

  void SetConfig(String key, long value) override {
    if (key == "matchTime") { matchSeconds = value; configs[0].value = value; }
    if (key == "flashTime") { flashMs      = value; configs[1].value = value; }
  }

  void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd) override {
    Serial.println("KotH Selected");
    this->timers  = timers;
    this->buttons = buttons;
    this->lcd     = lcd;
    this->flash   = false;

    this->buttons[ButtonEnum::Button0]->Subscribe(sBtn0, this);
    this->buttons[ButtonEnum::Button1]->Subscribe(sBtn1, this);
    this->buttons[ButtonEnum::Button2]->Subscribe(sBtn2, this);
    this->buttons[ButtonEnum::Button3]->Subscribe(sBtn3, this);

    Ticks_t gameTicks = (Ticks_t)matchSeconds * 1000;
    for (int i = 0; i < KotH_MaxGameTimers; i++) {
      gameTimers[i] = timers->Start(gameTicks, sGameOver, this, TimerType::Periodic);
      timers->Pause(gameTimers[i]);
    }
    gameTimers[KotH_UpdateTimer] = timers->Start(100, sUpdate, this, TimerType::Periodic);
  }

  void ButtonEvent(int button, ButtonData data) override {
    if (data.event == ButtonState::Release) {
      for (int i = 0; i < KotH_MaxTeams; i++) timers->Pause(gameTimers[i]);
      timers->Resume(gameTimers[button]);
      timers->Resume(gameTimers[KotH_GameTimer]);
      gameTimers[KotH_FlashTimer] = timers->Start(flashMs, sClearFlash, this, TimerType::OneShot);
      flash = true;
    }
  }

  void UpdateScreen() override {
    lcd->ClearScreen();
    Ticks_t t[KotH_MaxGameTimers];
    for (int i = 0; i < KotH_MaxTeams; i++) t[i] = timers->ElapsedTime(gameTimers[i]);
    t[KotH_GameTimer] = timers->RemainingTime(gameTimers[KotH_GameTimer]);

    lcd->WriteMessage(TickString(t[0]),            0, Alignment::Left,   false);
    lcd->WriteMessage(TickString(t[1]),            3, Alignment::Left,   false);
    lcd->WriteMessage(TickString(t[2]),            3, Alignment::Right,  false);
    lcd->WriteMessage(TickString(t[3]),            0, Alignment::Right,  false);
    lcd->WriteMessage(TickString(t[KotH_GameTimer]), 1, Alignment::Center, false);
    if (flash) lcd->WriteMessage("Captured!", 2, Alignment::Center, false);
  }

  void Pause() override {
    for (int i = 0; i < KotH_MaxTimers; i++) timers->Pause(gameTimers[i]);
  }

  void Resume() override {
    for (int i = 0; i < KotH_MaxTimers; i++) timers->Resume(gameTimers[i]);
  }

  void DeInit() override {
    for (int i = 0; i < KotH_MaxTimers; i++) timers->Stop(gameTimers[i]);
  }

  void Reset() override {
    Ticks_t gameTicks = (Ticks_t)matchSeconds * 1000;
    for (int i = 0; i < KotH_MaxGameTimers; i++) {
      timers->Stop(gameTimers[i]);
      gameTimers[i] = timers->Start(gameTicks, sGameOver, this, TimerType::Periodic);
      timers->Pause(gameTimers[i]);
    }
    timers->Resume(gameTimers[KotH_UpdateTimer]);
  }
};
