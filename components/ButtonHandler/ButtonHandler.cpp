#include <cstring>
#include <unordered_map>
#include "esp_log.h"

#include "ButtonHandler.hpp"

#define MAX_CALLBACKS 50

static const char *TAG = "ButtonHandler";

static const TickType_t DEBOUNCE_DELAY = pdMS_TO_TICKS(20);

ButtonHandler ButtonHandler::instance;

ButtonState ButtonHandler::GetState(const gpio_num_t pin)
{
   if(BitCheck(configuredPins, pin))
   {
      return (BitCheck(states, pin)) ? BUTTON_STATE_RELEASED : BUTTON_STATE_PRESSED;
   }
   return BUTTON_STATE_UNCONFIGURED;
}

void ButtonHandler::RegisterButton(const gpio_num_t pin)
{
   SET(configuredPins, pin);
}

void ButtonHandler::DeregisterButton(const gpio_num_t pin)
{
   UNSET(configuredPins, pin);
}

void ButtonHandler::StartPolling()
{
   if(!eventQueue)
   {
      eventQueue = xQueueCreate(20, sizeof(ButtonEvent));
   }
   if(!callbackMutex)
   {
      callbackMutex = xSemaphoreCreateMutex();
   }
   xTaskCreate(
      PollingTask,
      "Button Polling Task",
      2048,
      NULL,
      configMAX_PRIORITIES - 1,
      &pollingTaskHandle);
   xTaskCreate(
      EventHandlerTask,
      "Button Event Handler Task",
      2048,
      NULL,
      configMAX_PRIORITIES - 2,
      &eventHandlerTaskHandle);
}

void ButtonHandler::SuspendPolling()
{
   vTaskSuspend(pollingTaskHandle);
   vTaskSuspend(eventHandlerTaskHandle);
}

void ButtonHandler::StopPolling()
{
   vTaskDelete(pollingTaskHandle);
   vTaskDelete(eventHandlerTaskHandle);
}

void ButtonHandler::RegisterCallback(ButtonEventCallback callback, std::any context)
{
   eventCallbacks[callback] = context;
}

void ButtonHandler::DeregisterCallback(ButtonEventCallback callback)
{
   eventCallbacks.erase(callback);
}

void ButtonHandler::PollingTask(void *arg)
{
   uint32_t previousValue = 0;
   for(;;)
   {
      for(int pin = GPIO_NUM_0; pin < GPIO_NUM_MAX; pin++)
      {
         if(BitCheck(instance.configuredPins, pin))
         {
            gpio_get_level(static_cast<gpio_num_t>(pin)) ? SET(instance.states, pin) : UNSET(instance.states, pin);
            vTaskDelay(DEBOUNCE_DELAY);

            if(!BitCheck(previousValue, pin) && BitCheck(instance.states, pin))
            {
               SET(previousValue, pin);
               ESP_LOGD(TAG, "GPIO[%d] val: 1\n", pin);

               TickType_t timestamp = xTaskGetTickCount();
               ButtonEvent evt = {
                  .pin = static_cast<gpio_num_t>(pin),
                  .eventType = BUTTON_EVENT_TYPE_RELEASE,
                  .timestamp = timestamp * portTICK_RATE_MS
               };

               if(!xQueueSendToBack(instance.eventQueue, &evt, 0))
               {
                  ESP_LOGW(TAG, "OVERFLOW PRESS!\n");
               }
            }
            else if(BitCheck(previousValue, pin) && !BitCheck(instance.states, pin))
            {
               UNSET(previousValue, pin);
               ESP_LOGD(TAG, "GPIO[%d] val: 0\n", pin);

               uint32_t timestamp = xTaskGetTickCount();
               ButtonEvent evt = {
                  .pin = static_cast<gpio_num_t>(pin),
                  .eventType = BUTTON_EVENT_TYPE_PRESS,
                  .timestamp = timestamp * portTICK_RATE_MS
               };

               if(!xQueueSendToBack(instance.eventQueue, &evt, 0))
               {
                  ESP_LOGW(TAG, "OVERFLOW RELEASE!\n");
               }
            }
         }
      }
   }
}

void ButtonHandler::EventHandlerTask(void *arg)
{
   for(;;)
   {
      ButtonEvent evt;
      if(xQueueReceive(instance.eventQueue, &evt, portMAX_DELAY))
      {
         ESP_LOGD(TAG, "button: %d event: %d timestamp: %d\n", evt.pin, evt.eventType, evt.timestamp);
         xSemaphoreTake(instance.callbackMutex, 0);
         for(auto &[callback, context] : instance.eventCallbacks)
         {
            callback(context, evt);
         }
         xSemaphoreGive(instance.callbackMutex);
      }
   }
}
