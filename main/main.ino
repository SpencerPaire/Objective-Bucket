#include "Timers.h"
#include "Button.h"

static Timers timers;
Timer_t *blink;

#define BUTTON0_PIN D3
#define BUTTON1_PIN D4
#define BUTTON2_PIN D5
#define BUTTON3_PIN D6
#define BUTTON4_PIN D7
static int ledBlink = LED_BUILTIN ;


void ToggleLED(void *context)
{
    int led = *(int*)context;
    digitalWrite(led, !digitalRead(led));
    //Serial.println("Heartbeat");
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
    pinMode(LED_BUILTIN , OUTPUT);
    digitalWrite(LED_BUILTIN , true);
    Serial.println("Power On");

    blink = timers.Start(500, ToggleLED, &ledBlink, TimerType::Periodic);

    //static Button buttonReset = Button(BUTTON0_PIN, ButtonEvent, blink, &timers);
    //static Button buttonReset = Button(BUTTON1_PIN, ButtonEvent, blink, &timers);
    //static Button buttonReset = Button(BUTTON2_PIN, ButtonEvent, blink, &timers);
    //static Button buttonReset = Button(BUTTON3_PIN, ButtonEvent, blink, &timers);
    static Button buttonReset = Button(BUTTON4_PIN, ButtonEvent, blink, &timers);

    // Test code, non-functional
    // auto lambda = [](void) {ToggleLED();};
    // timers.Start(500, lambda, TimerType::Periodic );
}
void loop()
{
    timers.Run();
}