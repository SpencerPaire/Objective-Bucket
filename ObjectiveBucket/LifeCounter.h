#pragma once
#include "GameModeRunner.h"

// ============================================================
//  Life / Hit Counter
//  - Each button increments that team's counter
//  - Brief "Counted!" flash on screen after each press
//  - Configurable: starting count, flash duration
// ============================================================

#define LIFECOUNTER_TEAMS 4

enum LifeCounter_Timers {
  LifeCounter_UpdateTimer,
  LifeCounter_FlashTimer,
  LifeCounter_MaxTimers,
};

#define LIFE_DEFAULT_START_COUNT  0
#define LIFE_DEFAULT_FLASH_MS     300

class LifeCounter : public GameMode {

  // ---- static callbacks ----
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
  long flashMs;

public:
  bool flash = false;

  LifeCounter() {
    this->name = "Counters";
    startCount = LIFE_DEFAULT_START_COUNT;
    flashMs    = LIFE_DEFAULT_FLASH_MS;

    this->numConfigs = 2;
    this->configs[0] = { "startCount", "Starting Count", LIFE_DEFAULT_START_COUNT, 0, 20,   "hits" };
    this->configs[1] = { "flashTime",  "Flash Duration", LIFE_DEFAULT_FLASH_MS,    100, 2000, "ms"  };
  }

  void SetConfig(String key, long value) override {
    if (key == "startCount") { startCount = value; configs[0].value = value; }
    if (key == "flashTime")  { flashMs    = value; configs[1].value = value; }
  }

  void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd) override {
    Serial.println("Life Counters Selected");
    this->timers  = timers;
    this->buttons = buttons;
    this->lcd     = lcd;

    this->buttons[ButtonEnum::Button0]->Subscribe(sBtn0, this);
    this->buttons[ButtonEnum::Button1]->Subscribe(sBtn1, this);
    this->buttons[ButtonEnum::Button2]->Subscribe(sBtn2, this);
    this->buttons[ButtonEnum::Button3]->Subscribe(sBtn3, this);

    gameTimers[LifeCounter_UpdateTimer] = timers->Start(100, sUpdate, this, TimerType::Periodic);
    Reset();
  }

  void ButtonEvent(int button, ButtonData data) override {
    if (data.event == ButtonState::Press) {
      counts[button]++;
      flash = true;
      gameTimers[LifeCounter_FlashTimer] = timers->Start(flashMs, sClearFlash, this, TimerType::OneShot);
    }
  }

  void UpdateScreen() override {
    lcd->ClearScreen();

    lcd->WriteMessage("  " + String(counts[0]), 0, Alignment::Left,  false);
    lcd->WriteMessage(String(counts[3]) + "  ", 0, Alignment::Right, false);
    lcd->WriteMessage("  " + String(counts[1]), 3, Alignment::Left,  false);
    lcd->WriteMessage(String(counts[2]) + "  ", 3, Alignment::Right, false);

    if (flash) lcd->WriteMessage("Counted!", 1, Alignment::Center, false);
  }

  void Pause()  override {}
  void Resume() override {}

  void DeInit() override {
    for (int i = 0; i < LifeCounter_MaxTimers; i++) timers->Stop(gameTimers[i]);
  }

  void Reset() override {
    for (int i = 0; i < LIFECOUNTER_TEAMS; i++) counts[i] = startCount;
  }
};
