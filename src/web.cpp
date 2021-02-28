
#include "web.h"


void restServerRouting();
void handleRoot();
void handleNotFound();
void setCrossOrigin();
void enableTimelapse();
void getHelloWord();
void getJpg();
void getSettings();
String getContentType(String filename);

StaticJsonDocument<1000> jsonDocument;
char buffer[1000];

WebServer server(80);

Settings *websettings;

void web_init(Settings *settings_in) {
  //Calls the function handleRoot regardless of the server uri ex.(192.168.100.110/edit server uri is "/edit")

  server.begin();//starts the server
  Serial.println("HTTP server started");
    server.onNotFound(handleRoot);
  restServerRouting();
  websettings = settings_in;
}

void web_handleclient(void) {
  server.handleClient();
}

// Define routing
void restServerRouting() {
    server.on("/test", HTTP_GET, [&]() {
        server.send(200, F("text/html"),
            F("Success!"));
    });
    server.on(F("/helloWorld"), HTTP_GET, getHelloWord);
    server.on(F("/jpg"), HTTP_GET, getJpg);
    server.on(F("/getsettings"), HTTP_GET, getSettings);
    server.on(F("/enabletimelapse"), HTTP_GET, enableTimelapse);
    // server.on(F("/getscriptlist"), HTTP_GET, getScriptList);
}


void setCrossOrigin(){
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.sendHeader(F("Access-Control-Max-Age"), F("600"));
  server.sendHeader(F("Access-Control-Allow-Methods"), F("PUT,POST,GET,OPTIONS"));
  server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

void handleNotFound() {
  server.send(404, "", "");
}

void enableTimelapse() {
  websettings->timelapse_enabled = 1;
  server.send(200, "text/json", "{\"name\": \"Success\"}");
}
void getHelloWord() {
  server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

void getJpg() {
  config_camera(websettings);
  auto frame = esp32cam::capture();
  if (frame == nullptr) {
    Serial.println("CAPTURE FAIL");
    server.send(503, "", "");
    return;
  }
  Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
                static_cast<int>(frame->size()));

  server.setContentLength(frame->size());
  server.send(200, "image/jpeg");
  WiFiClient client = server.client();
  frame->writeTo(client);
}

void getSettings() {
  setCrossOrigin();
  Serial.println("Settings");
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


  jsonDocument["timelapse_enabled"] = websettings->timelapse_enabled;
  jsonDocument["next_time"] = websettings->next_time;
  jsonDocument["interval"] = websettings->interval;
  jsonDocument["current_set"] = websettings->current_set;
  jsonDocument["current_photo"] = websettings->current_photo;
  jsonDocument["resolution"] = websettings->resolution;
  jsonDocument["format"] = websettings->format;
  jsonDocument["autogain"] = websettings->autogain;
  jsonDocument["autoexposure"] = websettings->autoexposure;
  jsonDocument["gain"] = websettings->gain;
  jsonDocument["exposure"] = websettings->exposure;
  jsonDocument["verticalflip"] = websettings->verticalflip;
  jsonDocument["quality"] = websettings->quality;
  jsonDocument["gainceiling"] = websettings->gainceiling;
  jsonDocument["brightness"] = websettings->brightness;
  jsonDocument["lenscorrection"] = websettings->lenscorrection;
  jsonDocument["saturation"] = websettings->saturation;
  jsonDocument["contrast"] = websettings->contrast;
  jsonDocument["sharpness"] = websettings->sharpness;
  jsonDocument["horizontalflip"] = websettings->horizontalflip;
  jsonDocument["blackpixelcorrection"] = websettings->blackpixelcorrection;
  jsonDocument["whitepixelcorrection"] = websettings->whitepixelcorrection;
  serializeJson(jsonDocument, buffer);

  server.send(200, "application/json", buffer);

}


String getContentType(String filename) {
  if (server.hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
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
  fs::FS &fs = SD;
  String path = server.uri();
  Serial.print("path ");  Serial.println(path);

  if (path.endsWith("/")) {
    path += "index.html";
  }

  String contentType = getContentType(path);
  Serial.print("contentType ");
  Serial.println(contentType);
  File file = fs.open(path, "r");
  server.streamFile(file, contentType);
  file.close();
  // Serial.println("Root:");
  // handleNotFound();
}
