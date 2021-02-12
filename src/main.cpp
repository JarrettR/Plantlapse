
#include <Arduino.h>

#include "credentials.h"
#include "pins.h"
#include "storage.h"

#include <WiFi.h> 
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>

WebServer server(80);



void setup() {

  Serial.begin(115200);

  Serial.flush();
}


void loop() {

  Serial.print('.');
  vTaskDelay(120);
  
}
