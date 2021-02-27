
#include "settings.h"



Settings::Settings() {
  //
}

Settings::~Settings() {
  //
}

void Settings::begin() {
  dirty = 1;
  //Pull from NVS
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
