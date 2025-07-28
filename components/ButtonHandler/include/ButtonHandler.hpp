#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <any>
#include <functional>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "portmacro.h"

#define SET(bitmap, bit) bitmap |= BIT(bit)
#define UNSET(bitmap, bit) bitmap &= ~BIT(bit)

constexpr bool BitCheck(unsigned bitmap, unsigned bit)
{
   return (bitmap >> bit) & 1U;
}

enum ButtonState
{
   BUTTON_STATE_PRESSED,
   BUTTON_STATE_RELEASED,
   BUTTON_STATE_MAX,
   BUTTON_STATE_UNCONFIGURED
};

enum ButtonEventType
{
   BUTTON_EVENT_TYPE_PRESS = 0,
   BUTTON_EVENT_TYPE_RELEASE = 1,
   BUTTON_EVENT_TYPE_HOLD = 2,
   BUTTON_EVENT_TYPE_MAX
};

typedef void (*ButtonEventCallback)(std::any context, const std::any);

typedef struct
{
   gpio_num_t pin;
   ButtonEventType eventType;
   TickType_t timestamp;
} ButtonEvent;

class ButtonHandler
{
private:
   uint16_t states = 0;
   uint16_t configuredPins = 0;
   QueueHandle_t eventQueue = NULL;
   std::unordered_map<ButtonEventCallback, std::any> eventCallbacks;
   SemaphoreHandle_t callbackMutex = NULL;
   TaskHandle_t eventHandlerTaskHandle = NULL;
   TaskHandle_t pollingTaskHandle = NULL;

   static void PollingTask(void *arg);
   static void EventHandlerTask(void *arg);

public:
   static ButtonHandler instance;
   static ButtonHandler &GetInstance()
   {
      return instance;
   }
   ButtonState GetState(const gpio_num_t pin);
   void RegisterButton(const gpio_num_t pin);
   void DeregisterButton(const gpio_num_t pin);
   void StartPolling();
   void SuspendPolling();
   void StopPolling();
   void RegisterCallback(ButtonEventCallback callback, std::any context);
   void DeregisterCallback(ButtonEventCallback callback);
};

#endif // BUTTONHANDLER_H
