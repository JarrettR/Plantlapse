
#include <Arduino.h>


void setup() {

  Serial.begin(115200);

  Serial.flush();
}


void loop() {

  Serial.print('.');
  
}
