#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>
#include "Lcd.h"

#define LINE_LENGTH 20
#define NUM_ROWS 4
#define MAX_BUFFER_LENGTH (LINE_LENGTH + 1)

uint8_t slidingWindowStart = 0;
String msg = "Hello, World! Akilan Narayanaswamy";
Lcd lcd(4, 20);

void setup() {
  Serial.begin(9600);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);

  delay(5000);

  lcd.Init();
}

void scrollText(String message)
{
  uint8_t slidingWindowEnd = slidingWindowStart + LINE_LENGTH;
  String firstPart = message.substring(slidingWindowStart, slidingWindowEnd);
  lcd.WriteMessage(firstPart, 0, 0);
  if(slidingWindowEnd >= message.length())
  {
    slidingWindowStart = 0;
  }
  else
  {
    slidingWindowStart = (slidingWindowStart + 1) % LINE_LENGTH;
  }
}

void loop() {
  if (digitalRead(D5)) scrollText(msg);
  lcd.Update();
  delay(200);
}