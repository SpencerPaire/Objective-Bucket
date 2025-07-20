#include "driver/gpio.h"

#include "esp_log.h"

#include "ButtonHandler.h"
#include "portmacro.h"

#define GPIO_OUTPUT_IO_0 D4
#define GPIO_OUTPUT_PIN_SEL (BIT(GPIO_OUTPUT_IO_0))
#define GPIO_INPUT_IO_0 D7
#define GPIO_INPUT_PIN_SEL (BIT(GPIO_INPUT_IO_0))

static const char *TAG = "main";

void test(void *arg)
{
   gpio_set_level(GPIO_OUTPUT_IO_0, !gpio_get_level(GPIO_OUTPUT_IO_0));
}

extern "C"
{
   void app_main(void)
   {
      esp_log_level_set(TAG, ESP_LOG_DEBUG);
      gpio_config_t inputConfig;
      inputConfig.intr_type = GPIO_INTR_DISABLE;
      inputConfig.pin_bit_mask = GPIO_INPUT_PIN_SEL;
      inputConfig.mode = GPIO_MODE_INPUT;
      inputConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
      inputConfig.pull_up_en = GPIO_PULLUP_ENABLE;
      gpio_config(&inputConfig);

      gpio_config_t outputConfig;
      outputConfig.intr_type = GPIO_INTR_DISABLE;
      outputConfig.mode = GPIO_MODE_OUTPUT;
      outputConfig.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
      outputConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
      outputConfig.pull_up_en = GPIO_PULLUP_DISABLE;
      gpio_config(&outputConfig);

      ButtonHandler &buttonHandler = ButtonHandler::GetInstance();
      buttonHandler.RegisterButton(GPIO_INPUT_IO_0);
      buttonHandler.StartPolling();
      // buttonHandler.RegisterCallback(test, NULL);
   }
}
