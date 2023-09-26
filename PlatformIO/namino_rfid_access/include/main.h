#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include "pins_arduino.h"
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>


#define CS_PIN   16
#define RST_PIN  18
#define SCK_PIN  14
#define MISO_PIN 17
#define MOSI_PIN  9

#define LOOP_PERIOD   500           // 500ms loop period
#define SCREEN_ON    8000           //  s of screen on time
#define LINE_LEN    20              // Line Buffer Size
// #define FIELD_READ_PERIOD 2000      // 2s interval of field read

#endif // MAIN_H
