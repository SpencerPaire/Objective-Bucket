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

class GameMode {
  protected:
    String name;
    Timers *timers;
    Button **buttons;
    Lcd *lcd;

  public:
    GameMode();
    String GetName();
    virtual void Init(Timers *timers, Button *buttons[ButtonEnum_Max], Lcd *lcd);
    virtual void DeInit();
    virtual void Reset();
    virtual void ButtonEvent(int button, ButtonData data);
    virtual void UpdateScreen();
};

#define MaxGames 20

class GameModeRunner {
private:
  bool running;
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
  void AddGameMode(GameMode *game);
  void ButtonEvent(int button, ButtonData data);
};

#endif
