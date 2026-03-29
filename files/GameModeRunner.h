#ifndef GameMode_h
#define GameMode_h

#include "Arduino.h"
#include "Timers.h"
#include "Button.h"
#include "Lcd.h"
#include <functional>

typedef uint32_t Ticks_t;

enum ButtonEnum {
  Button0 = 0,
  Button1,
  Button2,
  Button3,
  ButtonReset,
  ButtonEnum_Max,
};

#define GAMEMODE_IDLE 0xFF

// Config entry exposed by each GameMode for web UI
struct GameConfig {
  String key;
  String label;
  long value;
  long minValue;
  long maxValue;
  String unit; // e.g. "sec", "lives"
};

#define MAX_CONFIG_ENTRIES 8

class GameMode {
  protected:
    String name;
    Timers *timers;
    Button **buttons;
    Lcd *lcd;

    GameConfig configs[MAX_CONFIG_ENTRIES];
    int numConfigs;

  public:
    GameMode();
    GameMode(String name);
    String GetName();

    // Config API - override in subclasses to expose settings
    virtual int GetConfigCount() { return numConfigs; }
    virtual GameConfig GetConfig(int index) { return configs[index]; }
    virtual void SetConfig(String key, long value) {}

    virtual void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
    virtual void DeInit();
    virtual void Pause();
    virtual void Resume();
    virtual void Reset();
    virtual void ButtonEvent(int button, ButtonData data);
    virtual void UpdateScreen();

    // State query for web UI
    virtual bool IsRunning() { return false; }
    virtual bool IsPaused() { return false; }
};

#define MaxGames 20

class GameModeRunner {
private:
  bool running;
  bool paused;
  int mode;
  Timers *timers;
  Button **buttons;
  Lcd *lcd;
  GameMode *games[MaxGames];
  int numGames;
  void SetButtons();
  void GameSelect();

public:
  GameModeRunner(Timers *timers = NULL, Button *buttons[ButtonEnum_Max] = NULL, Lcd *lcd = NULL);
  void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
  int CurrentMode();
  int GetNumGames();
  GameMode* GetGame(int index);
  bool IsRunning();
  bool IsPaused();
  void AddGameMode(GameMode *game);
  void ButtonEvent(int button, ButtonData data);

  // Web-callable actions
  void WebSelectGame(int index);
  void WebStartGame();
  void WebResetGame();
  void WebPauseGame();
  void WebResumeGame();
};

#endif
