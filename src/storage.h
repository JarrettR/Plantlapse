#pragma once

#include <Arduino.h>

#include "pins.h"

#include <FS.h>
#include <SD.h>
#include <SPI.h>


class Storage {
    public:
        Storage(SemaphoreHandle_t &_spi_sem);
        void begin(void);
        void getFileList(void);
        void writeFile(const char * path, const char * message);

    private:
        SemaphoreHandle_t &spi_sem;
        SPIClass spi;

};