#include <cstring>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "ButtonHandler.h"
#include "portmacro.h"

static const char *TAG = "ButtonHandler";

static const TickType_t DEBOUNCE_DELAY = pdMS_TO_TICKS(20);

static uint32_t buttonStates = 0;
static uint32_t configuredInputPins = 0;
static QueueHandle_t buttonEventQueue = NULL;

void ButtonHandler::RegisterButton(const gpio_num_t pin)
{
   SET(configuredInputPins, pin);
}

void ButtonHandler::DeregisterButton(const gpio_num_t pin)
{
   UNSET(configuredInputPins, pin);
}

void ButtonHandler::StartPolling()
{
   buttonEventQueue = xQueueCreate(20, sizeof(ButtonEvent_t));
   xTaskCreate(PollingTask, "Button Polling Task", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
   xTaskCreate(EventHandlerTask, "Button Event Handler Task", 2048, NULL, configMAX_PRIORITIES - 2, NULL);
}

void ButtonHandler::PollingTask(void *arg)
{
   uint32_t previousValue = 0;
   for(;;)
   {
      for(int pin = 0; pin < GPIO_NUM_MAX; pin++)
      {
         if(BitCheck(configuredInputPins, pin))
         {
            gpio_get_level(static_cast<gpio_num_t>(pin)) ? SET(buttonStates, pin) : UNSET(buttonStates, pin);
            vTaskDelay(DEBOUNCE_DELAY);
         }

         if(!BitCheck(previousValue, pin) && BitCheck(buttonStates, pin))
         {
            SET(previousValue, pin);
            ESP_LOGD(TAG, "GPIO[%d] intr, val: 1\n", pin);
            TickType_t timestamp = xTaskGetTickCount();
            ButtonEvent_t evt = { .eventType = BUTTON_EVENT_TYPE_PRESS, .timestamp = timestamp };
            if(!xQueueSendToBack(buttonEventQueue, &evt, 0))
            {
               ESP_LOGW(TAG, "OVERFLOW PRESS!\n");
            }
         }
         else if(BitCheck(previousValue, pin) && !BitCheck(buttonStates, pin))
         {
            UNSET(previousValue, pin);
            uint32_t timestamp = xTaskGetTickCount();
            ButtonEvent_t evt = { .eventType = BUTTON_EVENT_TYPE_RELEASE, .timestamp = timestamp };
            if(!xQueueSendToBack(buttonEventQueue, &evt, 0))
            {
               ESP_LOGW(TAG, "OVERFLOW RELEASE!\n");
            }
            ESP_LOGD(TAG, "GPIO[%d] intr, val: 0\n", pin);
         }
      }
   }
}

void ButtonHandler::EventHandlerTask(void *arg)
{
}
