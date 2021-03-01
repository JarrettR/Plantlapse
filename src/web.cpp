
#include "web.h"


void restServerRouting();
void handleRoot();
void handleNotFound();
void setCrossOrigin();
void enableTimelapse();
void disableTimelapse();
void getHelloWord();
void getJpg();
void getSettings();
void setSetting();
String getContentType(String filename);

StaticJsonDocument<2000> jsonDocument;
char buffer[2000];

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
    server.on(F("/now.jpg"), HTTP_GET, getJpg);
    server.on(F("/getsettings"), HTTP_GET, getSettings);
    server.on(F("/enabletimelapse"), HTTP_GET, enableTimelapse);
    server.on(F("/disabletimelapse"), HTTP_GET, disableTimelapse);
    server.on(F("/set"), HTTP_POST, setSetting);
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


void disableTimelapse() {
  websettings->timelapse_enabled = 0;
  server.send(200, "text/json", "{\"name\": \"Success\"}");
}

void getHelloWord() {
  server.send(200, "text/json", "{\"name\": \"Hello world\"}");
}

void setSetting() {
  setCrossOrigin();
  Serial.println("Setting config");
  if (server.hasArg("plain") == false) {
    Serial.println("Failed");
    return;
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  Serial.println(body);

  websettings->timelapse_enabled = jsonDocument["timelapse_enabled"];
  // websettings->next_time = jsonDocument["next_time"];
  websettings->interval = jsonDocument["interval"];
  websettings->current_set = jsonDocument["current_set"];
  websettings->current_photo = jsonDocument["current_photo"];

  websettings->autogain = jsonDocument["autogain"];
  websettings->autoexposure = jsonDocument["autoexposure"];
  websettings->gain = jsonDocument["gain"];
  websettings->exposure = jsonDocument["exposure"];
  websettings->verticalflip = jsonDocument["verticalflip"];
  websettings->quality = jsonDocument["quality"];
  websettings->gainceiling = jsonDocument["gainceiling"];
  websettings->brightness = jsonDocument["brightness"];
  websettings->lenscorrection = jsonDocument["lenscorrection"];
  websettings->saturation = jsonDocument["saturation"];
  websettings->contrast = jsonDocument["contrast"];
  websettings->sharpness = jsonDocument["sharpness"];
  websettings->horizontalflip = jsonDocument["horizontalflip"];
  websettings->blackpixelcorrection = jsonDocument["blackpixelcorrection"];
  websettings->whitepixelcorrection = jsonDocument["whitepixelcorrection"];

  // Respond to the client
  server.send(200, "application/json", "{}");
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
