#pragma once

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

class Settings
{
public:
  Settings();
  ~Settings();

  void setInterval(uint32_t value);
  uint32_t getInterval(void);


  bool timelapse_enabled;
  volatile bool ota_start;
  time_t next_time;
  //In ms
  time_t interval;
  uint32_t current_set;
  uint32_t current_photo;

  uint8_t resolution;
  uint8_t format;
  bool autogain;
  bool autoexposure;
  int gain;
  int exposure;
  bool verticalflip;
  int quality;
  int gainceiling;
  int brightness;
  bool lenscorrection;
  int saturation;
  int contrast;
  int sharpness;
  bool horizontalflip;
  bool blackpixelcorrection;
  bool whitepixelcorrection;


  private:
    //Unsaved changes
    bool dirty;

};