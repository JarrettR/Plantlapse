#pragma once

#include <Arduino.h>


class Settings
{
public:
  Settings();
  ~Settings();

  void setInterval(uint32_t value);
  uint32_t getInterval(void);


  uint8_t timelapse_enabled;
  time_t next_time;
  //In ms
  uint32_t interval;
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
    uint8_t dirty;

};
