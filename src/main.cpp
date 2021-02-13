
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
  

  // xTaskCreate(
  //   parseScript, // Task function
  //   "parseScript",           // String with name of task
  //   10000,              // Stack size in bytes
  //   NULL,               // Parameter passed as input of the task
  //   tskIDLE_PRIORITY+1, // Priority of the task
  //   NULL);    
}


void loop() {
  // server.handleClient();
  
  ntp.update();
  Serial.println(ntp.formattedTime("%d. %B %Y"));

  Serial.println(ntp.formattedTime("%A %T"));

  Serial.print('.');
  vTaskDelay(120);
  
}
