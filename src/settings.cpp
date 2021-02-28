
#include "settings.h"



Settings::Settings() {
  dirty = 1;
  contrast = 6;
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
