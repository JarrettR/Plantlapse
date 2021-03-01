
#include <Arduino.h>


#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <NTP.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#include "credentials.h"
#include "pins.h"
#include "settings.h"
#include "storage.h"
#include "web.h"

WiFiUDP wifiUdp;
NTP ntp(wifiUdp);
Settings settings;
Storage storage;

volatile bool _handlingOTA = false;

volatile time_t epochDiff;
time_t nextTime;

//30 seconds
const time_t interval = 30000;
void lapse(void * unused);
void updateEpoch(void * unused);

void preLapse();
void postLapse();

void setup() {

  Serial.begin(115200);

  Serial.flush();


  //Wifi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println(WiFi.localIP());
  // if (MDNS.begin("plantcam")) {
  //   Serial.println("mDNS responder started");
  // }
  ArduinoOTA.setHostname("plantcam");


  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      _handlingOTA = true;
      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  ntp.begin();

  web_init(&settings);

  storage.begin();

  camera_init();

    xTaskCreate(
      lapse, // Task function
      "lapse",           // String with name of task
      5000,              // Stack size in bytes
      NULL,               // Parameter passed as input of the task
      tskIDLE_PRIORITY+2, // Priority of the task
      NULL);

    xTaskCreate(
      updateEpoch, // Task function
      "updateEpoch",           // String with name of task
      2000,              // Stack size in bytes
      NULL,               // Parameter passed as input of the task
      tskIDLE_PRIORITY+1, // Priority of the task
      NULL);
}


void loop() {
  ArduinoOTA.handle();
  if(!_handlingOTA) {
    web_handleclient();
  }

  // Serial.print('.');
  // vTaskDelay(50);

}

void lapse(void * unused) {
  while(1) {
    if(_handlingOTA) {
      break;
    }
    if(settings.timelapse_enabled == true) {
      time_t current_time = (esp_timer_get_time() / 1000000) + epochDiff;
      char filename[40];
      sprintf(filename, "/%06d-%06d.jpg", settings.current_set, settings.current_photo);

      if (current_time >= settings.next_time) {
        Serial.println("Click! ");
        Serial.print(current_time);
        Serial.print(" ");
        Serial.print((int)(esp_timer_get_time() / 1000000));
        Serial.print(" ");
        Serial.print(epochDiff);
        Serial.print(" ");
        config_camera(&settings);
        auto frame = esp32cam::capture();
        if (frame == nullptr) {
          Serial.println("CAPTURE FAIL");
          break;
        }
        storage.writeFile(filename, frame->data(), frame->size());
        // storage.writeFile(filename, "Test-write");
        settings.current_photo++;
        settings.next_time = current_time + (settings.interval / 1000);
        Serial.println(settings.next_time);
      }
      time_t elapsed = ((esp_timer_get_time() / 1000000) + epochDiff) - current_time;
      if (elapsed > settings.interval) {
        elapsed = settings.interval;
      }

      vTaskDelay((settings.interval - elapsed) / portTICK_PERIOD_MS);
    } else {
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
  }
}

void updateEpoch(void * unused) {
  while(1) {
    if(_handlingOTA) {
      break;
    }
    Serial.print("Correcting time: ");
    ntp.update();
    epochDiff = ntp.epoch() - (esp_timer_get_time() / 1000000);
    Serial.println(ntp.epoch());
    // 30 minutes
    vTaskDelay(1800000 / portTICK_PERIOD_MS);
  }
}
