#include "camera.h"



static auto loRes = esp32cam::Resolution::find(320, 240);
static auto hiRes = esp32cam::Resolution::find(800, 600);


void config_camera(Settings* settings) {
  // esp32cam::Camera.changeResolution(loRes);
  //Format
  esp32cam::Camera.setAutoGain(1);
  esp32cam::Camera.setAutoExposure(1);
  esp32cam::Camera.setGain(settings->gain);
  esp32cam::Camera.setExposure(settings->exposure);
  esp32cam::Camera.setVerticalFlip(settings->verticalflip);
  esp32cam::Camera.setQuality(settings->quality);
  esp32cam::Camera.setGainCeiling(settings->gainceiling);
  esp32cam::Camera.setBrightness(settings->brightness);
  esp32cam::Camera.setLensCorrection(settings->lenscorrection);
  esp32cam::Camera.setSaturation(settings->saturation);
  esp32cam::Camera.setContrast(settings->contrast);
  esp32cam::Camera.setSharpness(settings->sharpness);
  esp32cam::Camera.setHorizontalFlip(settings->horizontalflip);
  esp32cam::Camera.setBlackPixelCorrection(settings->blackpixelcorrection);
  esp32cam::Camera.setWhitePixelCorrection(settings->whitepixelcorrection);
}
void serveJpg() {
  auto frame = esp32cam::capture();
//   if (frame == nullptr) {
//     Serial.println("CAPTURE FAIL");
//     server.send(503, "", "");
//     return;
//   }
//   Serial.printf("CAPTURE OK %dx%d %db\n", frame->getWidth(), frame->getHeight(),
//                 static_cast<int>(frame->size()));

//   server.setContentLength(frame->size());
//   server.send(200, "image/jpeg");
//   WiFiClient client = server.client();
//   frame->writeTo(client);
}
