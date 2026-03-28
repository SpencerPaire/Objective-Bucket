#ifndef LCD_H
#define LCD_H

#include "Wire.h"
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

enum class Alignment {
   Left,
   Center,
   Right,
};

class Lcd
{
   char **buffer;
   uint8_t cursorPosition;
   hd44780_I2Cexp lcd;
   const uint8_t numberOfRows, numberOfCols;
   char *blank;

public:
   Lcd(uint8_t numberOfRows, uint8_t numberOfCols);
   ~Lcd();
   void Init();
   void WriteMessage(String message, uint8_t row, uint8_t col, bool clearLine = true);
   void WriteMessage(String message, uint8_t row, Alignment align = Alignment::Left, bool clearLine = true);
   void Update();
   void ClearScreen();
   uint8_t GetRows();
   uint8_t GetCols();
};

#endif
