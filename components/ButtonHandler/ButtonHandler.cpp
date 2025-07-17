#include "ButtonHandler.h"

static const char *TAG = "ButtonHandler";

static const TickType_t DEBOUNCE_DELAY = pdMS_TO_TICKS(20);
static uint32_t buttonStates = 0;
static uint32_t configuredInputPins;

ButtonHandler &ButtonHandler::GetInstance()
{
   static ButtonHandler instance;
   return instance;
}

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
   xTaskCreate(PollingTask, "PollingTask", 2048, NULL, 10, NULL);
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
            // FreeRTOS Task notifications
            gpio_set_level(GPIO_OUTPUT_IO_0, !gpio_get_level(GPIO_OUTPUT_IO_0));
         }
         else if(BitCheck(previousValue, pin) && !BitCheck(buttonStates, pin))
         {
            UNSET(previousValue, pin);
            ESP_LOGD(TAG, "GPIO[%d] intr, val: 0\n", pin);
         }
      }
   }
}
