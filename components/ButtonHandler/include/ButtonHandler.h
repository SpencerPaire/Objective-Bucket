#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include "driver/gpio.h"
#include "portmacro.h"

#define D0 GPIO_NUM_16
#define D1 GPIO_NUM_5
#define D2 GPIO_NUM_4
#define D3 GPIO_NUM_0
#define D4 GPIO_NUM_2
#define D5 GPIO_NUM_14
#define D6 GPIO_NUM_12
#define D7 GPIO_NUM_13
#define D8 GPIO_NUM_15
#define RX GPIO_NUM_3
#define TX GPIO_NUM_1

#define SET(bitmap, bit) bitmap |= BIT(bit)
#define UNSET(bitmap, bit) bitmap &= ~BIT(bit)

constexpr bool BitCheck(unsigned bitmap, unsigned bit)
{
   return (bitmap >> bit) & 1U;
}

enum ButtonState_t
{
   BUTTON_STATE_PRESSED,
   BUTTON_STATE_RELEASED,
   BUTTON_STATE_MAX,
   BUTTON_STATE_UNCONFIGURED
};

enum ButtonEventType_t
{
   BUTTON_EVENT_TYPE_PRESS = 0,
   BUTTON_EVENT_TYPE_RELEASE = 1,
   BUTTON_EVENT_TYPE_HOLD = 2,
   BUTTON_EVENT_TYPE_MAX
};

typedef void (*ButtonEventCallback_t)(void *context, const void *arg);

typedef struct
{
   gpio_num_t pin;
   ButtonEventType_t eventType;
   TickType_t timestamp;
} ButtonEvent_t;

class ButtonHandler
{
private:
   static void PollingTask(void *arg);
   static void EventHandlerTask(void *arg);

public:
   static ButtonHandler &GetInstance()
   {
      static ButtonHandler instance;
      return instance;
   }
   ButtonState_t GetState(const gpio_num_t pin);
   void RegisterButton(const gpio_num_t pin);
   void DeregisterButton(const gpio_num_t pin);
   void StartPolling();
   void StopPolling();
   void RegisterCallback(ButtonEventCallback_t callback, void *context);
   void DeregisterCallback(ButtonEventCallback_t callback);
};

#endif // BUTTONHANDLER_H
