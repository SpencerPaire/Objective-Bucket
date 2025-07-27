#include "driver/gpio.h"

#include "ButtonHandler.h"
#include "esp_log.h"
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

#define LED_0_PIN D4
#define BUTTON_0_PIN D7

static const char *TAG = "main";

void test(std::any context, const std::any arg)
{
   auto buttonEvent = std::any_cast<const ButtonEvent>(arg);
   if(buttonEvent.pin == BUTTON_0_PIN)
   {
      if(buttonEvent.eventType == BUTTON_EVENT_TYPE_RELEASE)
      {
         gpio_set_level(LED_0_PIN, !gpio_get_level(LED_0_PIN));
      }
   }
}

extern "C"
{
   void app_main(void)
   {
      esp_log_level_set(TAG, ESP_LOG_DEBUG);
      gpio_config_t inputConfig = {
         .pin_bit_mask = BIT(BUTTON_0_PIN),
         .mode = GPIO_MODE_INPUT,
         .pull_up_en = GPIO_PULLUP_ENABLE,
         .pull_down_en = GPIO_PULLDOWN_DISABLE,
         .intr_type = GPIO_INTR_DISABLE,
      };
      gpio_config(&inputConfig);

      gpio_config_t outputConfig = {
         .pin_bit_mask = BIT(LED_0_PIN),
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
