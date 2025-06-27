#include "Lcd.h"

Lcd::Lcd(uint8_t numberOfRows, uint8_t numberOfCols)
   : numberOfRows(numberOfRows), numberOfCols(numberOfCols)
{
   buffer = new char *[numberOfRows];
   for(int i = 0; i < numberOfCols; i++)
   {
      buffer[i] = new char[numberOfCols + 1];
   }
   blank = new char[numberOfCols + 1];

   for(int i = 0; i < numberOfCols; i++)
   {
      blank[i] = ' ';
   }
   blank[numberOfCols] = 0;

   for(int i = 0; i < numberOfRows; i++)
   {
      strncpy(buffer[i], blank, numberOfCols + 1);
   }
}

void Lcd::Init()
{
   lcd.begin(numberOfCols, numberOfRows);
   lcd.backlight();
}

void Lcd::WriteMessage(String message, uint8_t row, uint8_t col)
{
   if(message.length() < numberOfCols)
   {
      for(int i = message.length(); i < numberOfCols; i++)
      {
         message.concat(' ');
      }
   }
   strncpy(buffer[row], blank, numberOfCols + 1);
   strncpy(&buffer[row][col], message.c_str(), message.length());
}

void Lcd::WriteMessageWithoutClear(String message, uint8_t row, uint8_t col)
{
   strncpy(&buffer[row][col], message.c_str(), message.length());
}

void Lcd::ClearScreen()
{
   for(int i = 0; i < numberOfRows; i++)
   {
      strncpy(buffer[i], blank, numberOfCols + 1);
   }
}

void Lcd::Update()
{
   for(int i = 0; i < numberOfRows; i++)
   {
      lcd.setCursor(0, i);
      lcd.print(buffer[i]);
   }
}

Lcd::~Lcd()
{
   delete buffer;
}

uint8_t Lcd::GetRows()
{
   return this->numberOfRows;
}

uint8_t Lcd::GetCols()
{
   return this->numberOfCols;
}
