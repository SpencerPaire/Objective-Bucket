#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncWebServer.h>
#include "Timers.h"
#include "Button.h"
#include "Lcd.h"
#include "GameModeRunner.h"
#include "WebServer.h"
#include "KotH.h"
#include "FifthElement.h"
#include "LifeCounter.h"

#define LINE_LENGTH 20
#define NUM_ROWS    4

static Timers timers;
static Lcd    lcd(NUM_ROWS, LINE_LENGTH);

#define BUTTON0_PIN D6
#define BUTTON1_PIN D7
#define BUTTON2_PIN D3
#define BUTTON3_PIN D4
#define BUTTON4_PIN D5

static OBWebServer *webServer = nullptr;

void UpdateLcd(void *context) { lcd.Update(); }

void setup()
{
  Serial.begin(9600);
  Serial.println("Power On");

  delay(5000);
  lcd.Init();
  lcd.WriteMessage("Objective Bucket", 0, Alignment::Center);
  lcd.WriteMessage("Starting WiFi...", 2, Alignment::Center);
  lcd.Update();

  static Button button0    = Button(BUTTON0_PIN, &timers);
  static Button button1    = Button(BUTTON1_PIN, &timers);
  static Button button2    = Button(BUTTON2_PIN, &timers);
  static Button button3    = Button(BUTTON3_PIN, &timers);
  static Button buttonReset = Button(BUTTON4_PIN, &timers);

  static Button *buttons[] = {
    &button0, &button1, &button2, &button3, &buttonReset,
  };

  static KotH         koth;
  static FifthElement fifthElement;
  static LifeCounter  lifecounter;
  static GameMode     separator(" --- ");

  static GameModeRunner runner(&timers, buttons, &lcd);
  runner.AddGameMode(&koth);
  runner.AddGameMode(&fifthElement);
  runner.AddGameMode(&lifecounter);
  runner.AddGameMode(&separator);

  static OBWebServer ws(&runner);
  webServer = &ws;
  webServer->Begin();

  // Show connection info briefly
  lcd.ClearScreen();
  lcd.WriteMessage("ob.local", 0, Alignment::Center);
  lcd.WriteMessage("192.168.4.1", 1, Alignment::Center);
  lcd.WriteMessage("\"Objective Bucket\"", 2, Alignment::Center);
  lcd.Update();
  delay(3000);

  lcd.ClearScreen();
  timers.Start(100, UpdateLcd, NULL, TimerType::Periodic);
}

void loop()
{
  timers.Run();
  if (webServer) webServer->Update(); // services mDNS
}
