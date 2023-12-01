#ifndef MAIN_H
#define MAIN_H
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <SPI.h>
#include <RTClib.h>


#define NAMINO_I2C_SCL   0 
#define NAMINO_I2C_SDA   1

#define LOOP_PERIOD   200           // 200ms loop period
#define SCREEN_ON   15000           //  s of screen on time
#define TIME_PERIOD  1000           // Read period of RTC
#define BLINK_ON     5000           // seconds of cursor blink

// Function Protos
void setBackLight(bool mode);
int readKeyboard();

#endif // MAIN_H
