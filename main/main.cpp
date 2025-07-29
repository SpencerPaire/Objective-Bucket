#include <any>
#include "FreeRTOS.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#include "esp_err.h"
#include "esp_log.h"
#include "portmacro.h"

#include "ButtonHandler.hpp"
#include "projdefs.h"

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

void i2c_task(void *arg)
{
   // TODO: I2C
   vTaskDelay(pdMS_TO_TICKS(100));
   static i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = D2,
      .sda_pullup_en = GPIO_PULLUP_DISABLE,
      .scl_io_num = D1,
      .scl_pullup_en = GPIO_PULLUP_DISABLE,
      .clk_stretch_tick = 300
   };
   ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, conf.mode));
   ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
   i2c_cmd_handle_t cmd = i2c_cmd_link_create();

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   i2c_master_write_byte(cmd, 0b00100011, 0x1);
   i2c_master_write_byte(cmd, 0b00000011, 0x1);
   i2c_master_stop(cmd);
   ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   vTaskDelay(pdMS_TO_TICKS(100));

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   i2c_master_write_byte(cmd, 0b00100011, 0x1);
   i2c_master_write_byte(cmd, 0b00000011, 0x1);
   i2c_master_stop(cmd);
   ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   vTaskDelay(pdMS_TO_TICKS(100));

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   i2c_master_write_byte(cmd, 0b00100011, 0x1);
   i2c_master_write_byte(cmd, 0b00000011, 0x1);
   i2c_master_stop(cmd);
   ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   vTaskDelay(pdMS_TO_TICKS(100));

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   i2c_master_write_byte(cmd, 0b00100010, 0x1);
   i2c_master_write_byte(cmd, 0b00000010, 0x1);
   i2c_master_stop(cmd);
   ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   vTaskDelay(pdMS_TO_TICKS(100));

   // i2c_master_start(cmd);
   // i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   // i2c_master_write_byte(cmd, 0b00100010, 0x1);
   // i2c_master_write_byte(cmd, 0b00000010, 0x1);
   // i2c_master_write_byte(cmd, 0b00101000, 0x1);
   // i2c_master_write_byte(cmd, 0b00001000, 0x1);
   // i2c_master_stop(cmd);
   // ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   // vTaskDelay(pdMS_TO_TICKS(100));

   // i2c_master_start(cmd);
   // i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   // i2c_master_write_byte(cmd, 0b00100000, 0x1);
   // i2c_master_write_byte(cmd, 0b00000000, 0x1);
   // i2c_master_write_byte(cmd, 0b00101000, 0x1);
   // i2c_master_write_byte(cmd, 0b00001000, 0x1);
   // i2c_master_stop(cmd);
   // ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   // vTaskDelay(pdMS_TO_TICKS(100));

   // i2c_master_start(cmd);
   // i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   // i2c_master_write_byte(cmd, 0b00100000, 0x1);
   // i2c_master_write_byte(cmd, 0b00000000, 0x1);
   // i2c_master_write_byte(cmd, 0b00100110, 0x1);
   // i2c_master_write_byte(cmd, 0b00000110, 0x1);
   // i2c_master_stop(cmd);
   // ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   // vTaskDelay(pdMS_TO_TICKS(100));

   // i2c_master_start(cmd);
   // i2c_master_write_byte(cmd, (0x27 << 1) | I2C_MASTER_WRITE, 0x1);
   // i2c_master_write_byte(cmd, 0b10100100, 0x1);
   // i2c_master_write_byte(cmd, 0b10000100, 0x1);
   // i2c_master_write_byte(cmd, 0b10101000, 0x1);
   // i2c_master_write_byte(cmd, 0b10001000, 0x1);
   // i2c_master_stop(cmd);
   // ESP_ERROR_CHECK(i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS));
   // vTaskDelay(pdMS_TO_TICKS(100));

   i2c_cmd_link_delete(cmd);

   for(;;)
   {
      vTaskDelay(pdMS_TO_TICKS(20));
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

      xTaskCreate(i2c_task, "I2C Task", 2048, NULL, 10, NULL);
   }
}
