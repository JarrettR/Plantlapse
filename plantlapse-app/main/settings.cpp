#include "include/settings.hpp"



Settings::Settings() {
  dirty = 1;

  timelapse_enabled = 0;
  ota_start = 0;
  next_time = 0;
  interval = 4000;
  current_set = 0;
  current_photo = 0;

  resolution = 0;
  format = 0;
  autogain = 0;
  autoexposure = 0;
  gain = 0;
  exposure = 0;
  verticalflip = 0;
  quality = 63;
  gainceiling = 0;
  brightness = 0;
  lenscorrection = 0;
  saturation = 0;
  contrast = 0;
  sharpness = 0;
  horizontalflip = 0;
  blackpixelcorrection = 0;
  whitepixelcorrection = 0;
}

Settings::~Settings() {
  //
}

void Settings::setInterval(uint32_t value) {
  dirty = 1;
  interval = value;
}

uint32_t Settings::getInterval() {
  return interval;
}

// void Settings::setContrast(int value) {
//   dirty = 1;
//   contrast = value;
// }
//
// int Settings::getContrast() {
//   return contrast;
// }