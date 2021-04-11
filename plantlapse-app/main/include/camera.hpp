#pragma once


#include "esp_camera.h"
#include "include/pins.h"
#include "esp_log.h"
#include "include/settings.hpp"

esp_err_t camera_init();
esp_err_t config_camera(Settings *settings_in);
esp_err_t camera_capture(char*);
 