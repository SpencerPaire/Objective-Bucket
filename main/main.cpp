#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp8266/eagle_soc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "portmacro.h"
#include "projdefs.h"

#include "ButtonHandler.h"

static const char *TAG = "main";

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
   }
}
