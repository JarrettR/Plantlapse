#pragma once

#include <Arduino.h>

#include "pins.h"

#include <FS.h>
#include <SD.h>
#include <SPI.h>


class Storage {
    public:
        Storage();
        void begin(void);
        void getFileList(void);
        void writeFile(const char * path, const char * message);
        void writeFile(const char * path, const uint8_t * message, size_t size);

    private:
        SPIClass spi;

};
