/* gpio example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "driver/gpio.h"

#include "esp_log.h"
#include "esp_system.h"
#include "portmacro.h"

static const char *TAG = "main";

/**
 * Brief:
 * This test code shows how to configure gpio and how to use gpio interrupt.
 *
 * GPIO status:
 * GPIO15: output
 * GPIO16: output
 * GPIO4:  input, pulled up, interrupt from rising edge and falling edge
 * GPIO5:  input, pulled up, interrupt from rising edge.
 *
 * Test:
 * Connect GPIO15 with GPIO4
 * Connect GPIO16 with GPIO5
 * Generate pulses on GPIO15/16, that triggers interrupt on GPIO4/5
 *
 */

#define GPIO_OUTPUT_IO_0 GPIO_NUM_2
#define GPIO_OUTPUT_IO_1 GPIO_NUM_16
#define GPIO_OUTPUT_PIN_SEL ((1ULL << GPIO_OUTPUT_IO_0) | (1ULL << GPIO_OUTPUT_IO_1))
#define GPIO_INPUT_IO_0 GPIO_NUM_13
#define GPIO_INPUT_IO_1 GPIO_NUM_0
#define GPIO_INPUT_PIN_SEL ((1ULL << GPIO_INPUT_IO_0) | (1ULL << GPIO_INPUT_IO_1))

#define NUM_BUTTONS 3

static xQueueHandle gpioEventQueue = NULL;
static TickType_t pressTime[NUM_BUTTONS] = { 0 };

class ButtonHandler
{
private:
public:
   ButtonHandler()
   {
      gpioEventQueue = xQueueCreate(10, sizeof(uint32_t));

      gpio_config_t inputConfig;
      inputConfig.intr_type = GPIO_INTR_POSEDGE;
      inputConfig.pin_bit_mask = GPIO_INPUT_PIN_SEL;
      inputConfig.mode = GPIO_MODE_INPUT;
      inputConfig.pull_up_en = GPIO_PULLUP_ENABLE;
      gpio_config(&inputConfig);

      gpio_config_t outputConfig;
      outputConfig.intr_type = GPIO_INTR_DISABLE;
      outputConfig.mode = GPIO_MODE_OUTPUT;
      outputConfig.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
      outputConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
      outputConfig.pull_up_en = GPIO_PULLUP_DISABLE;
      gpio_config(&outputConfig);

      gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);
      gpio_install_isr_service(0);
      gpio_isr_handler_add(GPIO_INPUT_IO_0, GpioIsrHandler, (void *)GPIO_INPUT_IO_0);

      xTaskCreate(GpioTaskExample, "GPIO Task Example", 2048, NULL, 10, NULL);
   }

   static void GpioIsrHandler(void *arg)
   {
      uint32_t gpio_num = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(arg));
      xQueueSendFromISR(gpioEventQueue, &gpio_num, NULL);
   }

   static void GpioTaskExample(void *arg)
   {
      uint32_t io_num;

      for(;;)
      {
         if(xQueueReceive(gpioEventQueue, &io_num, portMAX_DELAY))
         {
            TickType_t now = xTaskGetTickCount();
            if(now - pressTime[io_num] > 20)
            {
               ESP_LOGI(TAG, "GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(static_cast<gpio_num_t>(io_num)));
               gpio_set_level(GPIO_OUTPUT_IO_0, !gpio_get_level(GPIO_OUTPUT_IO_0));
               pressTime[io_num] = now;
            }
         }
      }
   }
};

extern "C"
{
   void app_main(void)
   {
      ButtonHandler();
      // int cnt = 0;

      //while(1)
      //{
         // ESP_LOGI(TAG, "cnt: %d\n", cnt++);
         // vTaskDelay(1000 / portTICK_RATE_MS);
         // gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
         // gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
      //}
   }
}
