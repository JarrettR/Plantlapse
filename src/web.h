#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <esp32cam.h>
#include <ArduinoJson.h>
// #include <FS.h>
// #include <SD.h>
// #include <SPI.h>
#include "storage.h"
#include "camera.h"
#include "settings.h"

void web_init(Settings *);
void web_handleclient(void);


class Web {
    public:
        Web();
        void handleClient(void);
        void getFileList(void);
        void writeFile(const char * path, const char * message);

    private:
        WebServer server;
        StaticJsonDocument<1000> jsonDocument;
        void setCrossOrigin(void);
        void getStatus();
        void getHelloWord();
        void getScriptList();
        void restServerRouting();
        String getContentType(String);

};
