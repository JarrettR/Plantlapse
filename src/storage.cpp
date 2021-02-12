
#include "storage.h"

Storage::Storage(SemaphoreHandle_t &_spi_sem) : spi_sem(_spi_sem), spi(HSPI) {

}

void Storage::begin() {
    if(xSemaphoreTake(spi_sem, (TickType_t)10) == pdTRUE) {
        spi.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS);
        SD.begin(PIN_SD_CS, spi);  
        if(!SD.begin(PIN_SD_CS)) {
            Serial.println("Card Mount Failed");
        }
        uint8_t cardType = SD.cardType();
        if(cardType == CARD_NONE) {
            Serial.println("No SD card attached");
        }
        Serial.println("Initializing SD card...");
        
        if (!SD.begin(PIN_SD_CS)) {
            Serial.println("ERROR - SD card initialization failed!");
        }

        File file = SD.open("/data.txt");
        if(!file) {
            Serial.println("File doesn't exist");
            Serial.println("Creating file...");
        }
        else {
            Serial.println("File already exists");  
        }
        file.close();
        xSemaphoreGive(spi_sem);
    }
}

void Storage::writeFile(const char * path, const char * message) {
    Serial.printf("Writing file: %s\n", path);

    if(xSemaphoreTake(spi_sem, (TickType_t)10) == pdTRUE) {
        File file = SD.open(path, FILE_WRITE);
        if(!file) {
            Serial.println("Failed to open file for writing");
            return;
        }
        if(file.print(message)) {
            Serial.println("File written");
        } else {
            Serial.println("Write failed");
        }
        file.close();
        xSemaphoreGive(spi_sem);
    }

}

void Storage::getFileList() {
    if(xSemaphoreTake(spi_sem, (TickType_t)10) == pdTRUE) {
        File dir = SD.open("/");

        while (true) {
        File entry =  dir.openNextFile();

        if (! entry) {
            break;
        }
        //char filename[69];
        //strncpy(filename, entry.name(), 69);
        Serial.println(entry.name());
        //Print filename here
        }
        xSemaphoreGive(spi_sem);
    }
}

// void getCSVList() {
//   setCrossOrigin();
//   jsonDocument.clear();

//   File dir = SD.open("/out");

//   while (true) {
//     File entry =  dir.openNextFile();

//     if (! entry) {
//       break;

//     }
//     char filename[69];
//     strncpy(filename, entry.name(), 69);
//     jsonDocument.add(filename);
//   }
//   serializeJson(jsonDocument, buffer);

//   server.send(200, "application/json", buffer);
// }