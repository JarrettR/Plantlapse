#pragma once


#include "esp_camera.h"
#include "include/pins.h"
#include "esp_log.h"

esp_err_t camera_init();
// esp_err_t camera_capture(size_t (*save)(const void*, size_t, size_t, FILE *), FILE * f);
esp_err_t camera_capture();
 