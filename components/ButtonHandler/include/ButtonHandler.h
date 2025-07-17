#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

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

#define GPIO_OUTPUT_IO_0 D4
#define GPIO_OUTPUT_PIN_SEL (BIT(GPIO_OUTPUT_IO_0))
#define GPIO_INPUT_IO_0 D7
#define GPIO_INPUT_PIN_SEL (BIT(GPIO_INPUT_IO_0))

#define SET(bitmap, bit) bitmap |= BIT(bit)
#define UNSET(bitmap, bit) bitmap &= ~BIT(bit)

constexpr bool BitCheck(unsigned bitmap, unsigned bit)
{
   return (bitmap >> bit) & 1U;
}

class ButtonHandler
{
private:
   static void PollingTask(void *arg);

public:
   static ButtonHandler &GetInstance();
   static void RegisterButton(const gpio_num_t pin);
   static void DeregisterButton(const gpio_num_t pin);
   static void StartPolling();
};

#endif // BUTTONHANDLER_H
