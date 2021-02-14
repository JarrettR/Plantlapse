#pragma once

#include <Arduino.h>
#include <WiFi.h> 
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ArduinoJson.h>



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
        void handleRoot();
        void getStatus();
        void getHelloWord();
        void getScriptList();
        void restServerRouting();
        String getContentType(String);

};