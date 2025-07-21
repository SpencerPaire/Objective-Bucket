#include "driver/gpio.h"

#include "esp_log.h"

#include "ButtonHandler.h"
#include "portmacro.h"

#define GPIO_OUTPUT_IO_0 D4
#define GPIO_OUTPUT_PIN_SEL (BIT(GPIO_OUTPUT_IO_0))
#define BUTTON_0_PIN D7
#define GPIO_INPUT_PIN_SEL (BIT(BUTTON_0_PIN))

static const char *TAG = "main";

void test(std::any context, const std::any arg)
{
   auto buttonEvent = std::any_cast<const ButtonEvent>(arg);
   if(buttonEvent.pin == BUTTON_0_PIN)
   {
      if(buttonEvent.eventType == BUTTON_EVENT_TYPE_RELEASE)
      {
         gpio_set_level(GPIO_OUTPUT_IO_0, !gpio_get_level(GPIO_OUTPUT_IO_0));
      }
   }
}

extern "C"
{
   void app_main(void)
   {
      esp_log_level_set(TAG, ESP_LOG_DEBUG);
      gpio_config_t inputConfig = {
         .pin_bit_mask = GPIO_INPUT_PIN_SEL,
         .mode = GPIO_MODE_INPUT,
         .pull_up_en = GPIO_PULLUP_ENABLE,
         .pull_down_en = GPIO_PULLDOWN_DISABLE,
         .intr_type = GPIO_INTR_DISABLE,
      };
      gpio_config(&inputConfig);

      gpio_config_t outputConfig = {
         .pin_bit_mask = GPIO_OUTPUT_PIN_SEL,
         .mode = GPIO_MODE_OUTPUT,
         .pull_up_en = GPIO_PULLUP_DISABLE,
         .pull_down_en = GPIO_PULLDOWN_DISABLE,
         .intr_type = GPIO_INTR_DISABLE
      };
      gpio_config(&outputConfig);

      ButtonHandler &buttonHandler = ButtonHandler::GetInstance();
      buttonHandler.RegisterButton(BUTTON_0_PIN);
      buttonHandler.StartPolling();
      buttonHandler.RegisterCallback(test, NULL);
   }
}
