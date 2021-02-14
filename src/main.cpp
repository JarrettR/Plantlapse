
#include <Arduino.h>


#include <WiFi.h> 
#include <WiFiUdp.h>
#include <NTP.h>

#include "credentials.h"
#include "pins.h"
#include "storage.h"
#include "web.h"


WiFiUDP wifiUdp;
NTP ntp(wifiUdp);
Web web;

time_t epochDiff;
void updateEpoch(void * unused);

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
  if (MDNS.begin("plantcam")) {
    Serial.println("mDNS responder started");
  }

  
  ntp.begin();
  

  xTaskCreate(
    updateEpoch, // Task function
    "updateEpoch",           // String with name of task
    1000,              // Stack size in bytes
    NULL,               // Parameter passed as input of the task
    tskIDLE_PRIORITY+1, // Priority of the task
    NULL);    
}


void loop() {
  web.handleClient();
  
  ntp.update();
  Serial.println(ntp.epoch());
  Serial.println(ntp.formattedTime("%d. %B %Y"));

  Serial.println(ntp.formattedTime("%A %T"));

  Serial.print('.');
  vTaskDelay(120);
  
}

void updateEpoch(void * unused) {
  ntp.update();
  epochDiff = ntp.epoch() - (esp_timer_get_time() / 1000000);
  // 30 minutes
  vTaskDelay(1800000 / portTICK_PERIOD_MS);
}