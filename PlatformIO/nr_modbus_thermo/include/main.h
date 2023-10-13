#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <SPI.h>                // SPI      Library
#include <Preferences.h>        // Preference Settings Library
#include <SD.h>                 // SD       Library
#include <RTClib.h>             // RTC      Library
#include <TFT_eSPI.h>           // TFT      Library
#include <Adafruit_SHT4x.h>     // SHT4x    Library


#define NAMINO_I2C_SCL         0 
#define NAMINO_I2C_SDA         1

#define LOOP_PERIOD          200            // 200ms loop period
#define SCREEN_ON           8000            //  s of screen on time
#define TIME_PERIOD         1000            // Read period of RTC
#define FIELD_READ_PERIOD   4000            // 4s interval of field read

#define CS_MICRO      10
#define CS_SD_CARD    14

#define TFT_BACKLIGHT 16
#define CALIBRATION_DATA    "pointercal"
#define CALIBRATION_POINTS  5

// Function Protos

#endif // MAIN_H
