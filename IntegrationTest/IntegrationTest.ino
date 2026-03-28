#include "Timers.h"
#include "Button.h"
#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>


#define LINE_LENGTH 20
#define NUM_ROWS 4
#define MAX_BUFFER_LENGTH (LINE_LENGTH + 1)

#define BUTTON0_PIN D5
#define BUTTON1_PIN D6
#define BUTTON2_PIN D7
#define BUTTON3_PIN D3
#define BUTTON4_PIN D4
static int ledBlink = LED_BUILTIN ;


hd44780_I2Cexp lcd;
uint8_t slidingWindowStart = 0;
String blank = "                    ";
String msg = "";
char lcdBuffer[NUM_ROWS][MAX_BUFFER_LENGTH];

static Timers timers;
Timer_t *blink;


void ToggleLED(void *context) {
    int led = *(int*)context;
    digitalWrite(led, !digitalRead(led));
    //Serial.println("Heartbeat");
}

void ButtonEvent(void *context, ButtonData data)
{
    int index = *(int*)context;
    if(data.event == ButtonState::Press)
    {
        String msg = "Button " + String(index);
        msg = msg + " at ";
        msg = msg + String(millis());

        strncpy(&lcdBuffer[index-1][0], msg.c_str(), msg.length());
        Serial.print("Button ");
        Serial.print(index);
        Serial.printf(" pressed after %d ms\n", data.releasedTime);
        timers.Pause(blink);
    }
    else if(data.event == ButtonState::Release)
    {
        Serial.print("Button ");
        Serial.print(index);
        Serial.printf(" released after %d ms\n", data.holdTime);
        timers.Resume(blink);
    }
    else if(data.event == ButtonState::Hold)
    {
        Serial.print("Button ");
        Serial.print(index);
        Serial.printf(" held for %d ms\n", data.holdTime);
    }
}


void updateLcd(void *context)
{
  for(int i = 0; i < NUM_ROWS; i++)
  {
    lcd.setCursor(0, i);
    lcdBuffer[i][MAX_BUFFER_LENGTH - 1] = 0;
    // Serial.println(lcdBuffer[i]);
    lcd.print(lcdBuffer[i]);
  }
}


void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN , OUTPUT);
    digitalWrite(LED_BUILTIN , true);
    Serial.println("Power On");
    
    for (uint8_t i = 0; i < NUM_ROWS; i++)
    {
        blank.toCharArray(lcdBuffer[i], MAX_BUFFER_LENGTH);
    }
    lcd.begin(20, 4);
    lcd.backlight();
    delay(5000);

    blink = timers.Start(500, ToggleLED, &ledBlink, TimerType::Periodic);
    timers.Start(200, updateLcd, NULL, TimerType::Periodic);
    static int iR = 0;
    static int i1 = 1;
    static int i2 = 2;
    static int i3 = 3;
    static int i4 = 4;
    static Button buttonR = Button(BUTTON0_PIN, &timers, ButtonEvent, &iR);
    static Button button1 = Button(BUTTON1_PIN, &timers, ButtonEvent, &i1);
    static Button button2 = Button(BUTTON2_PIN, &timers, ButtonEvent, &i2);
    static Button button3 = Button(BUTTON3_PIN, &timers, ButtonEvent, &i3);
    static Button button4 = Button(BUTTON4_PIN, &timers, ButtonEvent, &i4);

    // Test code, non-functional
    // auto lambda = [](void) {ToggleLED();};
    // timers.Start(500, lambda, TimerType::Periodic );
}

void loop()
{
    timers.Run();
}
