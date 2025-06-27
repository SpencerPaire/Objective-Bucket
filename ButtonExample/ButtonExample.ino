#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include "Timers.h"
#include "Button.h"
#include "Lcd.h"
#include "GameModeRunner.h"
#include "KotH.h"

#define LINE_LENGTH 20
#define NUM_ROWS 4
#define MAX_BUFFER_LENGTH (LINE_LENGTH + 1)

static Timers timers;
static Timer_t *blink;
static Lcd lcd(4, 20);

#define BUTTON0_PIN D3
#define BUTTON1_PIN D4
#define BUTTON2_PIN D5
#define BUTTON3_PIN D6
#define BUTTON4_PIN D7
static int ledBlink = LED_BUILTIN ;

void ToggleLED(void *context)
{
    // DOES NOT WORK, shared pin with button1
    int led = *(int*)context;
    digitalWrite(led, !digitalRead(led));
    //Serial.println("Heartbeat");
}

void UpdateLcd(void *context)
{
    lcd.Update();
}

void ButtonEvent(void *context, ButtonData data)
{
    Timer_t *blink = (Timer_t *)context;
    if(data.event == ButtonState::Press)
    {
        Serial.printf("Button pressed after %d ms\n", data.releasedTime);
        timers.Pause(blink);
    }
    else if(data.event == ButtonState::Release)
    {
        Serial.printf("Button released after %d ms\n", data.holdTime);
        timers.Resume(blink);
    }
    else if(data.event == ButtonState::Hold)
    {
        Serial.printf("Button held for %d ms\n", data.holdTime);
    }
}

void setup()
{
    Serial.begin(9600);
    //pinMode(ledBlink , OUTPUT);
    //digitalWrite(ledBlink , true);
    Serial.println("Power On");

    String msg = "Hello, World!";
    lcd.WriteMessage(msg, 0, 0);

    //blink = timers.Start(500, ToggleLED, &ledBlink, TimerType::Periodic);
    timers.Start(100, UpdateLcd, NULL, TimerType::Periodic);

    static Button button0 = Button(BUTTON0_PIN, &timers);
    static Button button1 = Button(BUTTON1_PIN, &timers);
    static Button button2 = Button(BUTTON2_PIN, &timers);
    static Button button3 = Button(BUTTON3_PIN, &timers);
    static Button buttonReset = Button(BUTTON4_PIN, &timers);

    static Button *buttons[] =
    {
        &button0,
        &button1,
        &button2,
        &button3,
        &buttonReset,
    };

    delay(5000);

    lcd.Init();

    lcd.ClearScreen();
    static GameModeRunner gameModeRunner = GameModeRunner(&timers, buttons, &lcd);

    static KotH koth = KotH();
    gameModeRunner.AddGameMode(&koth);

    static GameMode g1 = GameMode("Game 1");
    gameModeRunner.AddGameMode(&g1);

    static GameMode g2 = GameMode("Game 2");
    gameModeRunner.AddGameMode(&g2);
}

void loop()
{
    timers.Run();
}
