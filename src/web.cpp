
#include "web.h"



StaticJsonDocument<1000> jsonDocument;
WebServer server(80);


void web_init(){

  ////////////////Server
  server.onNotFound(handleRoot); //Calls the function handleRoot regardless of the server uri ex.(192.168.100.110/edit server uri is "/edit")
  server.begin();//starts the server
  Serial.println("HTTP server started");
  restServerRouting();
  // Set not found response
  // server.onNotFound(handleNotFound);
};

void setCrossOrigin(){
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.sendHeader(F("Access-Control-Max-Age"), F("600"));
  server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
  server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

void getHelloWord() {
  server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}
void getStatus() {
  setCrossOrigin();
  jsonDocument.clear();  
//   switch(status) {
//     case STATUS_RUNNING:
//       jsonDocument["status"] = "running";
//       break;
//     case STATUS_IDLE:
//     default:
//       jsonDocument["status"] = "idle";
//   }
//   jsonDocument["csv_filename"] = csv_filename;
//   jsonDocument["script_filename"] = script_filename;
//   jsonDocument["current_channel"] = current_channel;
//   jsonDocument["current_value"] = current_channel_value;
//   serializeJson(jsonDocument, buffer);

//   server.send(200, "application/json", buffer);
}

void getScriptList() {
  setCrossOrigin();
  jsonDocument.clear();

//   File dir = SD.open("/runscripts");

//   while (true) {
//     File entry =  dir.openNextFile();

//     if (! entry) {
//       break;

//     }
//     char filename[76];
//     strncpy(filename, entry.name(), 76);
//     jsonDocument.add(filename);
//   }
//   serializeJson(jsonDocument, buffer);

//   server.send(200, "application/json", buffer);
}


// Define routing
void restServerRouting() {
    server.on("/test", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Success!"));
    });
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
    server.on(F("/getstatus"), HTTP_GET, getStatus);
    server.on(F("/getscriptlist"), HTTP_GET, getScriptList);
    // server.on(F("/senddmx"), HTTP_POST, sendDMX);	
}

//This functions returns a String of content type
String getContentType(String filename) {
  if (server.hasArg("download")) { // check if the parameter "download" exists
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) { //check if the string filename ends with ".htm"
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "text/plain";
}

void handleRoot() {

  /* SD_MMC pertains to the sd card "memory". It is save as a
    variable at the same address given to fs in the fs library
    with "FS" class to enable the file system wrapper to make
    changes on the sd cards memory */
//   fs::FS &fs = SD;
//   String path = server.uri(); //saves the to a string server uri ex.(192.168.100.110/edit server uri is "/edit")
//   Serial.print("path ");  Serial.println(path);

//   //To send the index.html when the serves uri is "/"
//   if (path.endsWith("/")) {
//     path += "index.html";
//   }

//   //gets the extension name and its corresponding content type
//   String contentType = getContentType(path);
//   Serial.print("contentType ");
//   Serial.println(contentType);
//   File file = fs.open(path, "r"); //Open the File with file name = to path with intention to read it. For other modes see <a href="https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html" style="font-size: 13.5px;"> https://arduino-esp8266.readthedocs.io/en/latest/...</a>
//   server.streamFile(file, contentType); //sends the file to the server references from <a href="https://github.com/espressif/arduino-esp32/blob/master/libraries/WebServer/src/WebServer.h" style="font-size: 13.5px;"> https://arduino-esp8266.readthedocs.io/en/latest/...</a>
//   file.close(); //Close the file
}