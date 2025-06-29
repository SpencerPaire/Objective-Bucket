#include "Timers.h"
#include "Button.h"

static Timers timers;
Timer_t *blink;

#define BUTTON0_PIN D5
#define BUTTON1_PIN D6
#define BUTTON2_PIN D7
#define BUTTON3_PIN D3
#define BUTTON4_PIN D4
static int ledBlink = LED_BUILTIN ;


void ToggleLED(void *context)
{
    int led = *(int*)context;
    digitalWrite(led, !digitalRead(led));
    //Serial.println("Heartbeat");
}


void ButtonEvent(void *context, ButtonData data)
{
    int index = *(int*)context;
    if(data.event == ButtonState::Press)
    {
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
void setup()
{
    Serial.begin(9600);
    pinMode(LED_BUILTIN , OUTPUT);
    digitalWrite(LED_BUILTIN , true);
    Serial.println("Power On");

    blink = timers.Start(500, ToggleLED, &ledBlink, TimerType::Periodic);
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
