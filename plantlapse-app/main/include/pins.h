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

#define PIN_CAM_D0 5
#define PIN_CAM_D1 18
#define PIN_CAM_D2 19
#define PIN_CAM_D3 21
#define PIN_CAM_D4 36
#define PIN_CAM_D5 39
#define PIN_CAM_D6 34
#define PIN_CAM_D7 35
#define PIN_CAM_XCLK 0
#define PIN_CAM_PCLK 22
#define PIN_CAM_VSYNC 25
#define PIN_CAM_HREF 23
#define PIN_CAM_SDA 26
#define PIN_CAM_SCL 27
#define PIN_CAM_RESET -1
#define PIN_CAM_PWDN 32