#pragma once

#include <esp32cam.h>
#include "pins.h"
#include "settings.h"

void camera_init(void);
void config_camera(Settings*);
