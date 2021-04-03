#pragma once

#include "driver/gpio.h"


#define PIN_SD_CMD (gpio_num_t)15
#define PIN_SD_D0  (gpio_num_t)2
#define PIN_SD_D1  (gpio_num_t)4
#define PIN_SD_D2  (gpio_num_t)12
#define PIN_SD_D3  (gpio_num_t)13

#define PIN_SD_CS   (gpio_num_t)13
#define PIN_SD_MOSI (gpio_num_t)15
#define PIN_SD_CLK  (gpio_num_t)14
#define PIN_SD_MISO (gpio_num_t)2