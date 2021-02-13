#pragma once

#include <Arduino.h>
#include <WiFi.h> 
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>


void handleRoot();
void restServerRouting();
String getContentType(String);