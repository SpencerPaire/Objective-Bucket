#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

#define LINE_LENGTH 20
#define NUM_ROWS 4
#define MAX_BUFFER_LENGTH (LINE_LENGTH + 1)

hd44780_I2Cexp lcd;
uint8_t slidingWindowStart = 0;
String blank = "                    ";
String msg = "Hello, World! Akilan Narayanaswamy";
char lcdBuffer[NUM_ROWS][MAX_BUFFER_LENGTH];

void setup() {
  Serial.begin(9600);
  pinMode(D3, INPUT_PULLUP);
  pinMode(D5, INPUT_PULLUP);
  pinMode(D6, INPUT_PULLUP);
  pinMode(D7, INPUT_PULLUP);
  pinMode(D4, INPUT_PULLUP);

  for (uint8_t i = 0; i < NUM_ROWS; i++)
  {
    blank.toCharArray(lcdBuffer[i], MAX_BUFFER_LENGTH);
  }
  lcd.begin(20, 4);
  lcd.backlight();
  delay(5000);
}

void scrollText(String message)
{
  if (message.length() < 20)
  {
    for (uint8_t i = message.length(); i < LINE_LENGTH; i++)
    {
      message.concat(' ');
    }
  }
  blank.toCharArray(lcdBuffer[0], MAX_BUFFER_LENGTH);
  uint8_t slidingWindowEnd = slidingWindowStart + LINE_LENGTH;
  String firstPart = message.substring(slidingWindowStart, slidingWindowEnd);
  strncpy(&lcdBuffer[0][0], firstPart.c_str(), firstPart.length());
  if(slidingWindowEnd >= message.length())
  {
    slidingWindowStart = 0;
  }
  else
  {
    slidingWindowStart = (slidingWindowStart + 1) % LINE_LENGTH;
  }
}

void updateLcd()
{
  for(int i = 0; i < NUM_ROWS; i++)
  {
    lcd.setCursor(0, i);
    lcdBuffer[i][MAX_BUFFER_LENGTH - 1] = 0;
    lcd.print(lcdBuffer[i]);
  }
}

void loop() {
  if (digitalRead(D5)) scrollText(msg);
  updateLcd();
  delay(200);
}